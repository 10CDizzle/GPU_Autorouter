#pragma once

#include <wx/wx.h>
#include <wx/scrolwin.h>
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
    const std::vector<PcbLine>& GetLines() const { return m_lines; }
    const std::vector<PcbPad>& GetPads() const { return m_pads; }
    wxRect2DDouble GetBoundingBox() const;

private:
    std::vector<PcbLine> m_lines;
    std::vector<PcbPad> m_pads;
    wxRect2DDouble m_boundingBox;
};

struct SessionState {
    double scale = 1.0;
    bool isNightMode = false;
    bool loaded = false;
};

class PcbCanvas : public wxScrolled<wxPanel>
{
public:
    PcbCanvas(wxWindow* parent);

    void LoadKicadPcb(const wxString& path);
    void LoadSesFile(const wxString& path);
    void SaveFile(const wxString& path);
    SessionState LoadFile(const wxString& path);
    void ApplySessionState(const SessionState& state);
    void SetNightMode(bool nightMode);

private:
    void OnPaint(wxPaintEvent& event);
    void OnDraw(wxDC& dc);

    // Mouse events for panning and zooming
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnEnterWindow(wxMouseEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);

    // Keyboard events
    void OnKeyDown(wxKeyEvent& event);

    double m_scale;
    wxPoint m_panStartPos;
    wxPoint m_mouseLogicalPos; // For status bar updates

    PcbData m_pcbData;
    // Theming
    wxColour m_bgColour;
    wxColour m_gridColour;
    wxColour m_textColour;
};