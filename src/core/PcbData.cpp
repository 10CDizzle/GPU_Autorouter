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

    // Update bounding box
    if (m_lines.size() == 1 && m_pads.empty()) {
        m_boundingBox.SetLeft(line.start.m_x);
        m_boundingBox.SetTop(line.start.m_y);
        m_boundingBox.SetRight(line.start.m_x);
        m_boundingBox.SetBottom(line.start.m_y);
    }
    m_boundingBox.Union(line.start);
    m_boundingBox.Union(line.end);
}

void PcbData::AddPad(const PcbPad& pad)
{
    m_pads.push_back(pad);

    // Update bounding box based on pad's geometry
    wxPoint2DDouble topLeft(pad.pos.m_x - pad.size.m_x / 2.0, pad.pos.m_y - pad.size.m_y / 2.0);
    wxPoint2DDouble bottomRight(pad.pos.m_x + pad.size.m_x / 2.0, pad.pos.m_y + pad.size.m_y / 2.0);

    if (m_lines.empty() && m_pads.size() == 1) {
        m_boundingBox.SetLeft(topLeft.m_x);
        m_boundingBox.SetTop(topLeft.m_y);
        m_boundingBox.SetRight(bottomRight.m_x);
        m_boundingBox.SetBottom(bottomRight.m_y);
    }
    else {
        m_boundingBox.Union(topLeft);
        m_boundingBox.Union(bottomRight);
    }
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