#pragma once

#include <wx/wx.h>
#include <wx/scrolwin.h>
#include "../core/PcbData.h" // Include the refactored data header

class PcbCanvas : public wxScrolled<wxPanel>
{
public:
    PcbCanvas(wxWindow* parent);

    void LoadSesFile(const wxString& path);
    void SaveFile(const wxString& path);
    SessionState LoadFile(const wxString& path);
    void ApplySessionState(const SessionState& state);
    void SetNightMode(bool nightMode);

    void SetPcbData(const PcbData* data);
    void UpdateVirtualSize();

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

    const PcbData* m_pcbDataPtr;
    // Theming
    wxColour m_bgColour;
    wxColour m_gridColour;
    wxColour m_textColour;
};