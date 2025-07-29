#include "PcbData.h"
#include <algorithm>

void PcbData::Clear()
{
    m_lines.clear();
    m_pads.clear();
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