#include "PcbCanvas.h"
#include <wx/dcbuffer.h> // For wxBufferedPaintDC
#include <wx/textfile.h> // For wxTextFile

PcbCanvas::PcbCanvas(wxWindow* parent)
    : wxScrolled<wxPanel>(parent, wxID_ANY)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT); // Use custom paint handler to reduce flicker.

    SetNightMode(false); // Initialize with light mode colors
    m_scale = 1.0;

    // Set up scrolling. The virtual size is the total area we can scroll over.
    // We'll update this when a PCB is loaded. For now, a large default.
    SetVirtualSize(2000, 2000);
    SetScrollRate(1, 1); // 1 pixel per scroll unit for easier calculations.

    // Bind events to their handlers
    Bind(wxEVT_PAINT, &PcbCanvas::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &PcbCanvas::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &PcbCanvas::OnMouseUp, this);
    Bind(wxEVT_MOTION, &PcbCanvas::OnMouseMove, this);
    Bind(wxEVT_MOUSEWHEEL, &PcbCanvas::OnMouseWheel, this);
    Bind(wxEVT_KEY_DOWN, &PcbCanvas::OnKeyDown, this);
    Bind(wxEVT_ENTER_WINDOW, &PcbCanvas::OnEnterWindow, this);
    Bind(wxEVT_LEAVE_WINDOW, &PcbCanvas::OnLeaveWindow, this);
}

void PcbCanvas::LoadKicadPcb(const wxString& path)
{
    // Placeholder for actual KiCad file parsing logic.
    // In a real implementation, you would parse the file, populate your
    // data structures, determine the board's dimensions, and then
    // update the virtual size with SetVirtualSize().
    wxFrame* parentFrame = wxDynamicCast(GetParent(), wxFrame);
    if (parentFrame)
    {
        parentFrame->SetStatusText("Loaded KiCad file: " + path);
    }

    Refresh(); // Trigger a repaint to show the new data
}

void PcbCanvas::LoadSesFile(const wxString& path)
{
    // Placeholder for actual SES file parsing logic.
    wxFrame* parentFrame = wxDynamicCast(GetParent(), wxFrame);
    if (parentFrame)
    {
        parentFrame->SetStatusText("Loaded SES file: " + path);
    }
    Refresh();
}

void PcbCanvas::SaveFile(const wxString& path)
{
    // In a real implementation, you would serialize your PCB data
    // (layers, tracks, pads, nets, etc.) into your neutral format.
    wxTextFile file;
    if (!file.Create(path) && !file.Open(path))
    {
        wxLogError("Failed to create or open session file '%s'.", path);
        return;
    }

    file.AddLine("// Placeholder for neutral PCB routing session data");
    file.AddLine(wxString::Format("Scale=%.4f", m_scale));

    // Check if night mode is active by comparing background color
    bool isNightMode = (m_bgColour == wxColour(50, 50, 50));
    file.AddLine(wxString::Format("NightMode=%d", isNightMode));

    file.Write();
    file.Close();

    wxFrame* parentFrame = wxDynamicCast(GetParent(), wxFrame);
    if (parentFrame)
    {
        parentFrame->SetStatusText("Session saved to: " + path, 0);
    }
}

SessionState PcbCanvas::LoadFile(const wxString& path)
{
    SessionState state;
    wxTextFile file(path);
    if (!file.Open())
    {
        wxLogError("Failed to open session file '%s'.", path);
        return state;
    }

    for (size_t i = 0; i < file.GetLineCount(); ++i)
    {
        const wxString& line = file.GetLine(i);
        if (line.StartsWith("Scale="))
        {
            line.AfterFirst('=').ToDouble(&state.scale);
        }
        else if (line.StartsWith("NightMode="))
        {
            long nightModeVal = 0;
            line.AfterFirst('=').ToLong(&nightModeVal);
            state.isNightMode = (nightModeVal == 1);
        }
    }

    state.loaded = true;
    return state;
}

void PcbCanvas::ApplySessionState(const SessionState& state)
{
    m_scale = state.scale;
    SetNightMode(state.isNightMode);
    Refresh();
}

void PcbCanvas::SetNightMode(bool nightMode)
{
    if (nightMode)
    {
        m_bgColour = wxColour(50, 50, 50);
        m_gridColour = wxColour(80, 80, 80);
        m_textColour = wxColour(200, 200, 200);
    }
    else
    {
        m_bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
        m_gridColour = *wxGREY;
        m_textColour = *wxBLACK;
    }
    SetBackgroundColour(m_bgColour);
    Refresh();
}

void PcbCanvas::OnPaint(wxPaintEvent& event)
{
    // Use a buffered DC to reduce flicker, especially on Windows.
    wxBufferedPaintDC dc(this);
    OnDraw(dc);
}

void PcbCanvas::OnDraw(wxDC& dc)
{
    // This is our main drawing method.
    // It's vital to prepare the DC for the current scroll position.
    DoPrepareDC(dc);

    // Apply the zoom scale to the device context.
    dc.SetUserScale(m_scale, m_scale);

    // For now, let's just draw a simple grid and some text.
    // In the future, this is where you will iterate through your
    // PCB data structures (layers, tracks, pads) and draw them.
    dc.SetPen(wxPen(m_gridColour));
    for (int i = 0; i < 200; ++i) {
        dc.DrawLine(i * 10, 0, i * 10, 2000);
        dc.DrawLine(0, i * 10, 2000, i * 10);
    }

    dc.SetFont(*wxNORMAL_FONT);
    dc.SetTextForeground(m_textColour);
    dc.DrawText("PCB Drawing Area - No file loaded.", 20, 20);
}

void PcbCanvas::OnMouseDown(wxMouseEvent& event)
{
    SetFocus(); // Capture keyboard focus on click.
    m_panStartPos = event.GetPosition();
    CaptureMouse();
}

void PcbCanvas::OnMouseUp(wxMouseEvent& event)
{
    if (HasCapture()) ReleaseMouse();
}

void PcbCanvas::OnMouseMove(wxMouseEvent& event)
{
    // Update logical mouse position for status bar
    m_mouseLogicalPos = CalcUnscrolledPosition(event.GetPosition());

    // Panning logic
    if (event.Dragging() && event.LeftIsDown()) {
        wxPoint delta = event.GetPosition() - m_panStartPos;
        Scroll(GetViewStart().x - delta.x, GetViewStart().y - delta.y);
    }

    // Update status bar with coordinates
    wxFrame* parentFrame = wxDynamicCast(GetParent(), wxFrame);
    if (parentFrame)
    {
        wxString coords;
        coords.Printf("X: %d, Y: %d | Zoom: %.2f%%", m_mouseLogicalPos.x, m_mouseLogicalPos.y, m_scale * 100);
        parentFrame->SetStatusText(coords, 1); // Set text for the second field
    }
}

void PcbCanvas::OnMouseWheel(wxMouseEvent& event)
{
    double zoomFactor = 1.1;
    wxPoint mousePos = event.GetPosition();

    // The logical position on the virtual canvas that is under the mouse
    wxPoint logicalPos = CalcUnscrolledPosition(mousePos);
    wxPoint viewStart = GetViewStart();

    double oldScale = m_scale;

    if (event.GetWheelRotation() > 0)
    {
        m_scale *= zoomFactor;
    }
    else
    {
        m_scale /= zoomFactor;
    }

    // Calculate the new view start to keep the point under the mouse stationary
    wxPoint newViewStart;
    newViewStart.x = logicalPos.x + (viewStart.x - logicalPos.x) * (oldScale / m_scale);
    newViewStart.y = logicalPos.y + (viewStart.y - logicalPos.y) * (oldScale / m_scale);

    Scroll(newViewStart);
    Refresh(false); // Redraw the canvas with the new scale
}

void PcbCanvas::OnKeyDown(wxKeyEvent& event)
{
    int step = 10; // pixels to scroll
    wxPoint pt = GetViewStart();

    switch (event.GetKeyCode())
    {
        case WXK_LEFT:  pt.x -= step; break;
        case WXK_RIGHT: pt.x += step; break;
        case WXK_UP:    pt.y -= step; break;
        case WXK_DOWN:  pt.y += step; break;
        default:
            event.Skip(); // Allow other handlers to process the event
            return;
    }

    Scroll(pt);
}

void PcbCanvas::OnEnterWindow(wxMouseEvent& event)
{
    SetCursor(wxCursor(wxCURSOR_CROSS));
}

void PcbCanvas::OnLeaveWindow(wxMouseEvent& event)
{
    SetCursor(wxNullCursor);
}