#include "AutorouterCore.h"
#include <wx/textfile.h>
#include <wx/log.h>
#include <algorithm>
#include <map>

AutorouterCore::AutorouterCore()
{
}

bool AutorouterCore::LoadKicadPcb(const wxString& path)
{
    wxTextFile file(path);
    if (!file.Open())
    {
        wxLogError("Failed to open KiCad PCB file '%s'.", path);
        return false;
    }

    m_pcbData.Clear();
    std::map<int, wxString> netIdToNameMap;

    // First pass: build the net list and a map from file net ID to name
    for (size_t i = 0; i < file.GetLineCount(); ++i)
    {
        wxString line = file.GetLine(i).Trim();
        if (line.StartsWith("(net ")) {
            long netId;
            int idEndPos = line.find(' ', 5);
            if (line.Mid(5, idEndPos - 5).ToLong(&netId)) {
                int quoteStart = line.find('"');
                if (quoteStart != wxString::npos) {
                    int quoteEnd = line.find('"', quoteStart + 1);
                    wxString netName = line.Mid(quoteStart + 1, quoteEnd - (quoteStart + 1));
                    netIdToNameMap[netId] = netName;
                    m_pcbData.AddNet(netName); // This builds the canonical net list
                }
            }
        }
    }

    // Second pass: parse geometry and associate pads with our internal net indices
    for (size_t i = 0; i < file.GetLineCount(); ++i)
    {
        wxString line = file.GetLine(i).Trim();
        if (line.StartsWith("(gr_line") && line.Contains("(layer Edge.Cuts)"))
        {
            int startPos = line.find("(start");
            int endPos = line.find("(end");
            if (startPos != wxString::npos && endPos != wxString::npos)
            {
                wxString startCoords = line.Mid(startPos + 7, line.find(')', startPos) - (startPos + 7));
                wxString endCoords = line.Mid(endPos + 5, line.find(')', endPos) - (endPos + 5));

                double sx, sy, ex, ey;
                if (startCoords.BeforeFirst(' ').ToDouble(&sx) && startCoords.AfterFirst(' ').ToDouble(&sy) &&
                    endCoords.BeforeFirst(' ').ToDouble(&ex) && endCoords.AfterFirst(' ').ToDouble(&ey))
                {
                    PcbLine pcbLine;
                    pcbLine.start = wxPoint2DDouble(sx, sy);
                    pcbLine.end = wxPoint2DDouble(ex, ey);
                    m_pcbData.AddLine(pcbLine);
                }
            }
        }
        else if (line.StartsWith("(pad"))
        {
            PcbPad pad;
            bool isValid = true;

            int atPos = line.find("(at ");
            int sizePos = line.find("(size ");
            int layersPos = line.find("(layers ");

            if (atPos == wxString::npos || sizePos == wxString::npos || layersPos == wxString::npos) continue;

            wxString atStr = line.Mid(atPos + 4, line.find(')', atPos) - (atPos + 4));
            isValid &= atStr.BeforeFirst(' ').ToDouble(&pad.pos.m_x);
            isValid &= atStr.AfterFirst(' ').ToDouble(&pad.pos.m_y);

            wxString sizeStr = line.Mid(sizePos + 6, line.find(')', sizePos) - (sizePos + 6));
            isValid &= sizeStr.BeforeFirst(' ').ToDouble(&pad.size.m_x);
            isValid &= sizeStr.AfterFirst(' ').ToDouble(&pad.size.m_y);

            if (line.Contains(" rect ")) pad.shape = SHAPE_RECT;
            else if (line.Contains(" circle ")) pad.shape = SHAPE_CIRCLE;
            else if (line.Contains(" oval ")) pad.shape = SHAPE_OVAL;
            else isValid = false;

            wxString layersStr = line.Mid(layersPos + 8, line.find(')', layersPos) - (layersPos + 8));
            if (layersStr.Contains("F.Cu")) pad.layer = "F.Cu";
            else if (layersStr.Contains("B.Cu")) pad.layer = "B.Cu";
            else isValid = false;

            int netPos = line.find("(net ");
            if (netPos != wxString::npos) {
                wxString netNumStr = line.Mid(netPos + 5, line.find(')', netPos) - (netPos + 5));
                long fileNetId;
                if (netNumStr.ToLong(&fileNetId) && netIdToNameMap.count(fileNetId)) {
                    const wxString& netName = netIdToNameMap[fileNetId];
                    pad.net_id = m_pcbData.GetNetIdByName(netName);
                }
            }

            if (isValid) m_pcbData.AddPad(pad);
        }
    }

    return true;
}

RoutingResult AutorouterCore::Route(const RoutingSettings& settings, const wxArrayInt& netsToRoute)
{
    RoutingResult result;
    wxStopWatch sw;

    // In a real implementation, you would determine the grid resolution
    // based on design rules (trace width, clearance).
    // For now, let's assume a simple scaling factor.
    const double grid_resolution = 0.1; // e.g., 10 grid cells per mm

    wxRect2DDouble bbox = m_pcbData.GetBoundingBox();
    int grid_width = static_cast<int>(ceil(bbox.GetRight() / grid_resolution));
    int grid_height = static_cast<int>(ceil(bbox.GetBottom() / grid_resolution));

    // Get all pads once
    const auto& allPads = m_pcbData.GetPads();

        // --- Create and populate the grid ONCE for efficiency ---
    RoutingGrid grid(grid_width, grid_height, grid_resolution);
    // Add all pads on the board as obstacles initially
    for (const auto& pad : allPads)
    {
        grid.AddPadObstacle(pad, false);
    }

    int routed_count = 0;
    double total_length = 0;
    int via_count = 0; // Not implemented yet

    // --- Route each net requested ---
    for (int netId : netsToRoute)
    {
        // 1. Find all pads for the current net
        std::vector<const PcbPad*> netPads;
        for (const auto& pad : allPads)
        {
            if (pad.net_id == netId)
            {
                netPads.push_back(&pad);
            }
        }

        // For now, we only handle simple 2-pin connections.
        // A real router would handle multi-pin nets with a more complex
        // algorithm (e.g., building a minimum spanning tree).
        if (netPads.size() < 2)
        {
            continue;
        }

        // 2. Temporarily mark the start and end pads as non-obstacles
        const PcbPad* startPad = netPads[0];
        const PcbPad* endPad = netPads[1];

        RoutingGrid grid(grid_width, grid_height, grid_resolution);

        // Add all pads on the board as obstacles
        grid.AddPadObstacle(*startPad, true);
        grid.AddPadObstacle(*endPad, true);

        // 3. Find the path using A*
        GridPoint startPoint = grid.WorldToGrid(startPad->pos);
        GridPoint endPoint = grid.WorldToGrid(endPad->pos);

        std::vector<GridPoint> path = grid.FindPath(startPoint, endPoint);

        // 4. If a path was found, update statistics
        if (!path.empty())
        {
            routed_count++;

            // Calculate path length
            double pathLen = 0;
            for (size_t i = 1; i < path.size(); ++i)
            {
                double dx = path[i].x - path[i-1].x;
                double dy = path[i].y - path[i-1].y;
                pathLen += std::sqrt(dx*dx + dy*dy);
            }
            total_length += pathLen * grid_resolution;
        }
                // Restore the pads as obstacles for the next net's calculation
        grid.AddPadObstacle(*startPad, false);
        grid.AddPadObstacle(*endPad, false);
    }

    long time = sw.Time();

    // --- Populate the result struct with actual calculated values ---
    result.time_ms = time;
    result.nets_total = netsToRoute.GetCount();
    result.nets_routed = routed_count;
    result.success = (result.nets_routed == result.nets_total);
    result.total_track_length = total_length;
    result.via_count = via_count;

    return result;
}

void AutorouterCore::RunGpuRouting(const RoutingSettings& settings)
{
    // This is a placeholder for the real, complex work.
    // We'll simulate the work with a sleep.
    wxMilliSleep(1500);
}