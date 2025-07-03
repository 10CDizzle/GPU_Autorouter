#pragma once

#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <vector>

// A simple struct to hold a line segment from the PCB file
struct PcbLine
{
    wxPoint start;
    wxPoint end;
};

// A class to hold all the parsed PCB data
class PcbData
{
public:
    void Clear();
    void AddLine(const PcbLine& line);
    const std::vector<PcbLine>& GetLines() const { return m_lines; }
    wxRect GetBoundingBox() const;

private:
    std::vector<PcbLine> m_lines;
    wxRect m_boundingBox;
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