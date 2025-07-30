#include "PcbCanvas.h"
#include <wx/dcbuffer.h> // For wxBufferedPaintDC
#include <wx/textfile.h> // For wxTextFile
#include "../core/AutorouterCore.h"

PcbCanvas::PcbCanvas(wxWindow* parent)
    : wxScrolled<wxPanel>(parent, wxID_ANY)
{
    m_pcbDataPtr = nullptr;
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
        m_gridColour = wxColour(192, 192, 192); // Standard grey
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

    const double pcb_scale = 10.0; // Scale factor: 10 pixels per mm

    if (!m_pcbDataPtr || (m_pcbDataPtr->GetLines().empty() && m_pcbDataPtr->GetPads().empty()))
    {
        // Draw placeholder text if no file is loaded
        dc.SetFont(*wxNORMAL_FONT);
        dc.SetTextForeground(m_textColour);
        dc.DrawText("Open a KiCad PCB file to view.", 20, 20);
    }
    else
    {
        // --- Draw Pads ---
        wxBrush oldBrush = dc.GetBrush();
        wxPen oldPen = dc.GetPen();
        dc.SetPen(*wxTRANSPARENT_PEN); // No outline for pads

        for (const auto& pad : m_pcbDataPtr->GetPads())
        {
            if (pad.layer == "F.Cu") dc.SetBrush(wxBrush(wxColour(200, 0, 0))); // Red for top copper
            else if (pad.layer == "B.Cu") dc.SetBrush(wxBrush(wxColour(0, 0, 200))); // Blue for bottom copper
            else continue;

            // KiCad 'at' is center, wxWidgets drawing is top-left. Convert and scale.
            double x = (pad.pos.m_x - pad.size.m_x / 2.0) * pcb_scale;
            double y = (pad.pos.m_y - pad.size.m_y / 2.0) * pcb_scale;
            double w = pad.size.m_x * pcb_scale;
            double h = pad.size.m_y * pcb_scale;

            // C++ switch statements do not work on strings. Use if-else if instead.
            if (pad.shape == "rect")
            {
                dc.DrawRectangle(wxPoint(x, y), wxSize(w, h));
            }
            else if (pad.shape == "circle" || pad.shape == "oval")
            {
                dc.DrawEllipse(wxPoint(x, y), wxSize(w, h));
            }
        }
        dc.SetBrush(oldBrush);
        dc.SetPen(oldPen);

        // Draw the PCB outline
        dc.SetPen(wxPen(wxColour(255, 255, 0), 2 / m_scale)); // Bright yellow for outline, scale pen width
        for (const auto& line : m_pcbDataPtr->GetLines())
        {
            dc.DrawLine(line.start.m_x * pcb_scale, line.start.m_y * pcb_scale, line.end.m_x * pcb_scale, line.end.m_y * pcb_scale);
        }
    }
}

void PcbCanvas::SetPcbData(const PcbData* data)
{
    m_pcbDataPtr = data;
    UpdateVirtualSize();
    Refresh();
}

void PcbCanvas::UpdateVirtualSize()
{
    if (!m_pcbDataPtr) return;

    const double pcb_scale = 10.0; // Scale factor: 10 pixels per mm
    wxRect2DDouble bbox = m_pcbDataPtr->GetBoundingBox();
    bbox.m_x -= 10.0; bbox.m_y -= 10.0;
    bbox.m_width += 20.0; bbox.m_height += 20.0;
    SetVirtualSize(bbox.GetRight() * pcb_scale, bbox.GetBottom() * pcb_scale);
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