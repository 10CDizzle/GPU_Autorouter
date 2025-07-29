#pragma once

#include <wx/wx.h>
#include <wx/scrolwin.h>
#include "../core/PcbData.h"

class PcbPanel : public wxScrolledWindow
{
public:
    PcbPanel(wxWindow* parent);

    void SetPcbData(const PcbData* data);

    void ZoomIn();
    void ZoomOut();
    void ZoomToFit();
    void SetNightMode(bool nightMode);

protected:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);

private:
    void DrawPcb(wxDC& dc);
    wxPoint2DDouble ScreenToWorld(const wxPoint& screenPos) const;
    wxPoint WorldToScreen(const wxPoint2DDouble& worldPos) const;

    const PcbData* m_pcbData = nullptr;
    double m_scale = 1.0;
    wxPoint2DDouble m_panOffset{0.0, 0.0}; // Pan offset in world coordinates (mm)

    // For panning
    bool m_isPanning = false;
    wxPoint m_panStartPos;

    // Theming
    wxColour m_bgColour;
    wxColour m_boardOutlineColour;
    wxColour m_padColourTop;
    wxColour m_padColourBottom;
    wxColour m_textColour;

    wxDECLARE_EVENT_TABLE();
};