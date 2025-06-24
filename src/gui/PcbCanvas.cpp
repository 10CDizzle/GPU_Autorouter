#include "PcbCanvas.h"

PcbCanvas::PcbCanvas(wxWindow* parent)
    : wxScrolled<wxPanel>(parent, wxID_ANY)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT); // Use custom paint handler to reduce flicker
    SetBackgroundColour(*wxLIGHT_GREY);

    // Set up scrolling. The virtual size is the total area we can scroll over.
    // We'll update this when a PCB is loaded. For now, a large default.
    SetVirtualSize(2000, 2000);
    SetScrollRate(10, 10); // 10 pixels per scroll unit

    // Bind events to their handlers
    Bind(wxEVT_PAINT, &PcbCanvas::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &PcbCanvas::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &PcbCanvas::OnMouseUp, this);
    Bind(wxEVT_MOTION, &PcbCanvas::OnMouseMove, this);
}

void PcbCanvas::LoadKicadPcb(const wxString& path)
{
    // Placeholder for actual KiCad file parsing logic.
    // In a real implementation, you would parse the file, populate your
    // data structures, determine the board's dimensions, and then
    // update the virtual size with SetVirtualSize().
    GetParent()->SetStatusText("Loaded KiCad file: " + path);

    Refresh(); // Trigger a repaint to show the new data
}

void PcbCanvas::LoadSesFile(const wxString& path)
{
    // Placeholder for actual SES file parsing logic.
    GetParent()->SetStatusText("Loaded SES file: " + path);
    Refresh();
}

void PcbCanvas::OnPaint(wxPaintEvent& event)
{
    // Use a wxPaintDC to ensure drawing only happens in the update region.
    wxPaintDC dc(this);
    OnDraw(dc);
}

void PcbCanvas::OnDraw(wxDC& dc)
{
    // This is our main drawing method.
    // It's vital to prepare the DC for the current scroll position.
    DoPrepareDC(dc);

    // For now, let's just draw a simple grid and some text.
    // In the future, this is where you will iterate through your
    // PCB data structures (layers, tracks, pads) and draw them.
    dc.SetPen(*wxGREY_PEN);
    for (int i = 0; i < 200; ++i) {
        dc.DrawLine(i * 10, 0, i * 10, 2000);
        dc.DrawLine(0, i * 10, 2000, i * 10);
    }

    dc.SetFont(*wxNORMAL_FONT);
    dc.SetTextForeground(*wxBLACK);
    dc.DrawText("PCB Drawing Area - No file loaded.", 20, 20);
}

void PcbCanvas::OnMouseDown(wxMouseEvent& event)
{
    m_panStartPos = event.GetPosition();
    CaptureMouse();
}

void PcbCanvas::OnMouseUp(wxMouseEvent& event)
{
    if (HasCapture()) ReleaseMouse();
}

void PcbCanvas::OnMouseMove(wxMouseEvent& event)
{
    if (event.Dragging() && event.LeftIsDown()) {
        wxPoint delta = event.GetPosition() - m_panStartPos;
        Scroll(GetViewStart().x - delta.x, GetViewStart().y - delta.y);
    }
}