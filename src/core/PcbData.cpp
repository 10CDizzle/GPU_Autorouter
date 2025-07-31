#include "PcbData.h"
#include <algorithm>
#include <set>

void PcbData::Clear()
{
    m_lines.clear();
    m_pads.clear();
    m_vias.clear();
    m_zones.clear();
    m_nets.clear();
    m_boundingBox = wxRect2DDouble();
}

void PcbData::AddLine(const PcbLine& line)
{
    m_lines.push_back(line);
    m_boundingBox.Union(line.start);
    m_boundingBox.Union(line.end);
}

void PcbData::AddPad(const PcbPad& pad)
{
    m_pads.push_back(pad);

    // Update bounding box based on pad's geometry.
    // wxRect2DDouble::Union handles being called on an uninitialized (default)
    // rectangle correctly.
    wxRect2DDouble padRect(pad.pos.m_x - pad.size.m_x / 2.0,
                           pad.pos.m_y - pad.size.m_y / 2.0,
                           pad.size.m_x,
                           pad.size.m_y);
    m_boundingBox.Union(padRect);
}

void PcbData::AddVia(const PcbVia& via)
{
    m_vias.push_back(via);
    m_boundingBox.Union(wxRect2DDouble(via.pos.m_x - via.size / 2.0,
                                       via.pos.m_y - via.size / 2.0,
                                       via.size, via.size));
}

void PcbData::AddZone(const PcbZone& zone)
{
    m_zones.push_back(zone);
    // Update bounding box from polygon points
    for (const auto& pt : zone.polygon) {
        m_boundingBox.Union(pt);
    }
}

std::vector<wxString> PcbData::GetUniqueLayerNames() const
{
    std::set<wxString> layerSet;

    for (const auto& line : m_lines) {
        layerSet.insert(line.layer);
    }
    for (const auto& pad : m_pads) {
        // Special handling for non-copper layers
        if (pad.shape == "np_thru_hole") {
            layerSet.insert("Hole");
        } else {
            layerSet.insert(pad.layer);
        }
    }
    for (const auto& via : m_vias) {
        // Vias are drawn as a single type for now
        layerSet.insert("Via");
    }
    for (const auto& zone : m_zones) {
        layerSet.insert(zone.layer);
    }

    std::vector<wxString> uniqueLayers(layerSet.begin(), layerSet.end());
    std::sort(uniqueLayers.begin(), uniqueLayers.end());
    return uniqueLayers;
}

wxRect2DDouble PcbData::GetBoundingBox() const
{
    return m_boundingBox;
}

void PcbData::AddNet(const wxString& netName)
{
    // Avoid adding duplicates or empty nets
    if (!netName.IsEmpty() && std::find(m_nets.begin(), m_nets.end(), netName) == m_nets.end())
    {
        m_nets.push_back(netName);
    }
}

int PcbData::GetNetIdByName(const wxString& netName) const
{
    auto it = std::find(m_nets.begin(), m_nets.end(), netName);
    if (it != m_nets.end())
    {
        return std::distance(m_nets.begin(), it);
    }
    return -1; // Not found
}