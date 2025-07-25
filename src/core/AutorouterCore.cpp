#include "AutorouterCore.h"
#include <wx/textfile.h>
#include <wx/log.h>
#include <algorithm>

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

            if (isValid) m_pcbData.AddPad(pad);
        }
        else if (line.StartsWith("(net "))
        {
            int quoteStart = line.find('"');
            if (quoteStart != wxString::npos)
            {
                int quoteEnd = line.find('"', quoteStart + 1);
                m_pcbData.AddNet(line.Mid(quoteStart + 1, quoteEnd - (quoteStart + 1)));
            }
        }
    }

    return true;
}

RoutingResult AutorouterCore::Route(const RoutingSettings& settings, const wxArrayInt& netsToRoute)
{
    RoutingResult result;
    wxStopWatch sw;

    // --- 1. Placeholder for the actual GPU routing work ---
    // This is where you would initialize the GPU, upload board data,
    // run the routing kernel, and retrieve the results.
    RunGpuRouting(settings);
    // ---

    long time = sw.Time();

    // --- 2. Evaluate results ---
    // For this test harness, we'll just generate some plausible dummy data.
    result.time_ms = time;
    result.nets_total = netsToRoute.GetCount();
    // Simulate 95% completion rate
    result.nets_routed = static_cast<int>(result.nets_total * 0.95);
    result.success = (result.nets_routed == result.nets_total);

    // Simulate some efficiency metrics
    result.total_track_length = 1234.5; // dummy value in mm
    result.via_count = 87; // dummy value

    return result;
}

void AutorouterCore::RunGpuRouting(const RoutingSettings& settings)
{
    // This is a placeholder for the real, complex work.
    // In a real implementation, this would involve:
    // 1. Creating a routing grid based on the PCB data.
    // 2. Initializing CUDA/OpenCL context.
    // 3. Uploading the grid, pads, and netlist to the GPU.
    // 4. Launching the pathfinder kernel for a number of passes.
    // 5. Downloading the resulting track and via data.
    // 6. Storing the routed geometry back into PcbData.

    // We'll simulate the work with a sleep.
    wxMilliSleep(1500);
}