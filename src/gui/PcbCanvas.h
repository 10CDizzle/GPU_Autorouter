#pragma once

#include <wx/wx.h>
#include <wx/scrolwin.h>

class PcbCanvas : public wxScrolled<wxPanel>
{
public:
    PcbCanvas(wxWindow* parent);

    void LoadKicadPcb(const wxString& path);
    void LoadSesFile(const wxString& path);
    void SaveFile(const wxString& path);

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
};