#pragma once

#include <wx/wx.h>
#include <wx/scrolwin.h>
#include "LayerColors.h"
#include "../core/PcbData.h" // Include the refactored data header

// Structure to hold session state when loading/saving
struct SessionState
{
    bool loaded = false;
    double scale = 1.0;
    bool isNightMode = false;
};

// Define some constants for pad shapes to avoid magic strings in the drawing code.
#define SHAPE_RECT "rect"
#define SHAPE_CIRCLE "circle"
#define SHAPE_OVAL "oval"

// Define a custom event for when the zoom-to-area action is complete.
wxDECLARE_EVENT(EVT_ZOOM_AREA_COMPLETE, wxCommandEvent);

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
    void ZoomIn();
    void ZoomOut();
    void ZoomToFit();
    void EnterZoomAreaMode();
    LayerColors& GetLayerColors() { return m_layerColors; }

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
    LayerColors m_layerColors;
    bool m_isNightMode;

    // For zoom-to-area
    bool m_isZoomingArea;
    wxPoint m_zoomAreaStart;
    wxRect m_zoomAreaRect;
};