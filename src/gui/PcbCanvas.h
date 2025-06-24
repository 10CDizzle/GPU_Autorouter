#pragma once

#include <wx/wx.h>
#include <wx/scrolwin.h>

class PcbCanvas : public wxScrolled<wxPanel>
{
public:
    PcbCanvas(wxWindow* parent);

    void LoadKicadPcb(const wxString& path);
    void LoadSesFile(const wxString& path);

private:
    void OnPaint(wxPaintEvent& event);
    void OnDraw(wxDC& dc);

    // Mouse events for panning
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);

    wxPoint m_panStartPos;
};