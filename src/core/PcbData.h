#pragma once

#include <wx/wx.h>
#include <vector>

// --- Data Structures for PCB Geometry ---

enum PadShape {
    SHAPE_RECT,
    SHAPE_CIRCLE,
    SHAPE_OVAL
};

struct PcbPad
{
    wxPoint2DDouble pos;    // Center position in mm
    wxPoint2DDouble size;   // Size in mm
    PadShape shape;
    wxString layer;
};

struct PcbLine
{
    wxPoint2DDouble start;
    wxPoint2DDouble end;
};

// A class to hold all the parsed PCB data
class PcbData
{
public:
    void Clear();
    void AddLine(const PcbLine& line);
    void AddPad(const PcbPad& pad);
    void AddNet(const wxString& netName);
    const std::vector<PcbLine>& GetLines() const { return m_lines; }
    const std::vector<PcbPad>& GetPads() const { return m_pads; }
    const std::vector<wxString>& GetNets() const { return m_nets; }
    wxRect2DDouble GetBoundingBox() const;

private:
    std::vector<PcbLine> m_lines;
    std::vector<PcbPad> m_pads;
    std::vector<wxString> m_nets;
    wxRect2DDouble m_boundingBox;
};

// This is also generic data, not strictly GUI
struct SessionState {
    double scale = 1.0;
    bool isNightMode = false;
    bool loaded = false;
};