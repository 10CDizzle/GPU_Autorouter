#include "PcbCanvas.h"
#include <wx/dcbuffer.h> // For wxBufferedPaintDC
#include <wx/textfile.h> // For wxTextFile
#include <wx/settings.h> // For wxSystemSettings
#include "../core/AutorouterCore.h"

// Define the custom event type
wxDEFINE_EVENT(EVT_ZOOM_AREA_COMPLETE, wxCommandEvent);

PcbCanvas::PcbCanvas(wxWindow* parent)
    : wxScrolled<wxPanel>(parent, wxID_ANY)
{
    m_pcbDataPtr = nullptr;
    m_isNightMode = false;
    m_isZoomingArea = false;
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
    m_isNightMode = nightMode;
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

void PcbCanvas::ZoomIn()
{
    double zoomFactor = 1.25;
    wxSize clientSize = GetClientSize();
    // Zoom towards the center of the current view
    wxPoint mousePos = wxPoint(clientSize.x / 2, clientSize.y / 2);

    wxPoint logicalPos = CalcUnscrolledPosition(mousePos);
    wxPoint viewStart = GetViewStart();
    double oldScale = m_scale;
    m_scale *= zoomFactor;

    wxPoint newViewStart;
    newViewStart.x = logicalPos.x - (mousePos.x / m_scale);
    newViewStart.y = logicalPos.y - (mousePos.y / m_scale);

    Scroll(newViewStart);
    Refresh(false);
}

void PcbCanvas::ZoomOut()
{
    double zoomFactor = 1.25;
    wxSize clientSize = GetClientSize();
    wxPoint mousePos = wxPoint(clientSize.x / 2, clientSize.y / 2);

    wxPoint logicalPos = CalcUnscrolledPosition(mousePos);
    wxPoint viewStart = GetViewStart();
    double oldScale = m_scale;
    m_scale /= zoomFactor;

    wxPoint newViewStart;
    newViewStart.x = logicalPos.x - (mousePos.x / m_scale);
    newViewStart.y = logicalPos.y - (mousePos.y / m_scale);

    Scroll(newViewStart);
    Refresh(false);
}

void PcbCanvas::EnterZoomAreaMode()
{
    m_isZoomingArea = true;
    SetCursor(wxCursor(wxCURSOR_CROSS));
}

void PcbCanvas::OnPaint(wxPaintEvent& event)
{
    // Use a buffered DC to reduce flicker, especially on Windows.
    wxBufferedPaintDC dc(this);

    // Explicitly clear the buffer with our background color first.
    // This prevents "ghosting" when panning or zooming because the
    // buffer is not automatically cleared on all platforms.
    dc.SetBackground(wxBrush(m_bgColour));
    dc.Clear();

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

    if (!m_pcbDataPtr || (m_pcbDataPtr->GetLines().empty() && m_pcbDataPtr->GetPads().empty() && m_pcbDataPtr->GetVias().empty()))
    {
        // Draw placeholder text if no file is loaded
        dc.SetFont(*wxNORMAL_FONT);
        dc.SetTextForeground(m_textColour);
        dc.DrawText("Open a KiCad PCB file to view.", 20, 20);
    }
    else
    {
        wxBrush oldBrush = dc.GetBrush();
        wxPen oldPen = dc.GetPen();

        // --- 1. Draw Zones (Copper Pours) ---
        dc.SetPen(*wxTRANSPARENT_PEN); // No outline for zones
        for (const auto& zone : m_pcbDataPtr->GetZones()) {
            if (!m_layerColors.IsVisible(zone.layer)) continue;
            wxColour zoneColour = m_layerColors.GetColour(zone.layer, m_isNightMode);
            // Make it semi-transparent for a "pour" look
            wxColour pourColour(zoneColour.Red(), zoneColour.Green(), zoneColour.Blue(), 80);
            dc.SetBrush(wxBrush(pourColour, wxBRUSHSTYLE_SOLID));

            std::vector<wxPoint> points;
            points.reserve(zone.polygon.size());
            for (const auto& pt : zone.polygon) {
                points.emplace_back(pt.m_x * pcb_scale, pt.m_y * pcb_scale);
            }
            if (!points.empty()) {
                dc.DrawPolygon(points.size(), points.data());
            }
        }

        // --- 2. Draw Traces (Copper Segments) ---
        for (const auto& line : m_pcbDataPtr->GetLines())
        {
            // Skip board outline for now, we'll draw it last
            if (!m_layerColors.IsVisible(line.layer) || line.layer == "Edge.Cuts") continue;
            dc.SetPen(wxPen(m_layerColors.GetColour(line.layer, m_isNightMode), line.width * pcb_scale, wxPENSTYLE_SOLID));
            dc.DrawLine(line.start.m_x * pcb_scale, line.start.m_y * pcb_scale, line.end.m_x * pcb_scale, line.end.m_y * pcb_scale);
        }

        // --- 3. Draw Pads ---
        dc.SetPen(*wxTRANSPARENT_PEN); // No outline for pads
        for (const auto& pad : m_pcbDataPtr->GetPads())
        {
            // Handle non-plated through-holes
            if (pad.shape == "np_thru_hole") {
                 if (!m_layerColors.IsVisible("Hole")) continue;
                 double drill_size = pad.size.m_x; // For npth, size is the drill size
                 dc.SetBrush(*wxBLACK_BRUSH);
                 dc.SetPen(wxPen(m_layerColors.GetColour("Hole", m_isNightMode), 1));
                 dc.DrawCircle(pad.pos.m_x * pcb_scale, pad.pos.m_y * pcb_scale, (drill_size / 2.0) * pcb_scale);
                 continue;
            }

            dc.SetBrush(wxBrush(m_layerColors.GetColour(pad.layer, m_isNightMode)));
            if (!m_layerColors.IsVisible(pad.layer)) continue;

            // KiCad 'at' is center, wxWidgets drawing is top-left. Convert and scale.
            double x = (pad.pos.m_x - pad.size.m_x / 2.0) * pcb_scale;
            double y = (pad.pos.m_y - pad.size.m_y / 2.0) * pcb_scale;
            double w = pad.size.m_x * pcb_scale;
            double h = pad.size.m_y * pcb_scale;

            if (pad.shape == "rect")
            {
                dc.DrawRectangle(wxPoint(x, y), wxSize(w, h));
            }
            else if (pad.shape == "circle" || pad.shape == "oval")
            {
                dc.DrawEllipse(wxPoint(x, y), wxSize(w, h));
            }
        }

        // --- 4. Draw Vias (on top of pads/traces) ---
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(m_layerColors.GetColour("Via", m_isNightMode)));
        if (m_layerColors.IsVisible("Via")) {
            for (const auto& via : m_pcbDataPtr->GetVias()) {
                dc.DrawCircle(via.pos.m_x * pcb_scale, via.pos.m_y * pcb_scale, (via.size / 2.0) * pcb_scale);
            }
        }

        // --- 5. Draw the PCB outline (last, so it's on top of everything) ---
        dc.SetPen(wxPen(m_layerColors.GetColour("Edge.Cuts", m_isNightMode), 2 / m_scale)); // Bright yellow for outline, scale pen width
        for (const auto& line : m_pcbDataPtr->GetLines())
        {
            if (line.layer == "Edge.Cuts" && m_layerColors.IsVisible("Edge.Cuts")) {
                dc.DrawLine(line.start.m_x * pcb_scale, line.start.m_y * pcb_scale, line.end.m_x * pcb_scale, line.end.m_y * pcb_scale);
            }
        }
        dc.SetBrush(oldBrush);
        dc.SetPen(oldPen);

        // --- 6. Draw Zoom-to-Area rubber band overlay ---
        if (m_isZoomingArea && (m_zoomAreaRect.width != 0 || m_zoomAreaRect.height != 0))
        {
            // The rubber band rect is in screen coordinates. We need to draw it on the DC
            // which is scrolled and scaled. We can convert the screen rect to logical coordinates.
            wxPoint scroll = GetViewStart();
            wxRect logicalRect;
            logicalRect.x = (m_zoomAreaRect.x + scroll.x) / m_scale;
            logicalRect.y = (m_zoomAreaRect.y + scroll.y) / m_scale;
            logicalRect.width = m_zoomAreaRect.width / m_scale;
            logicalRect.height = m_zoomAreaRect.height / m_scale;

            dc.SetPen(wxPen(*wxWHITE, 2 / m_scale, wxPENSTYLE_DOT));
            dc.DrawRectangle(logicalRect);
        }
    }
}

void PcbCanvas::SetPcbData(const PcbData* data)
{
    m_pcbDataPtr = data;
    UpdateVirtualSize();
    Refresh();
}

void PcbCanvas::ZoomToFit()
{
    if (!m_pcbDataPtr || m_pcbDataPtr->GetBoundingBox().IsEmpty()) return;

    wxRect2DDouble bbox = m_pcbDataPtr->GetBoundingBox();
    wxSize clientSize = GetClientSize();

    // Add some padding
    bbox.Inset(-bbox.m_width * 0.05, -bbox.m_height * 0.05);

    if (bbox.m_width <= 0 || bbox.m_height <= 0 || clientSize.x <= 0 || clientSize.y <= 0) return;

    const double pcb_to_pixels = 10.0;
    double scaleX = clientSize.x / (bbox.m_width * pcb_to_pixels);
    double scaleY = clientSize.y / (bbox.m_height * pcb_to_pixels);

    m_scale = std::min(scaleX, scaleY);

    // Center the view on the bounding box
    wxPoint2DDouble bboxCenter(bbox.m_x + bbox.m_width / 2.0, bbox.m_y + bbox.m_height / 2.0);
    wxPoint scrollTarget = wxPoint(bboxCenter.m_x * pcb_to_pixels * m_scale - clientSize.x / 2,
                                   bboxCenter.m_y * pcb_to_pixels * m_scale - clientSize.y / 2);
    Scroll(scrollTarget);
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
    if (m_isZoomingArea)
    {
        m_zoomAreaStart = event.GetPosition();
        m_zoomAreaRect.SetPosition(m_zoomAreaStart);
        m_zoomAreaRect.SetSize(wxSize(0, 0));
        CaptureMouse();
    }
    else
    {
        m_panStartPos = event.GetPosition();
        CaptureMouse();
    }
}

void PcbCanvas::OnMouseUp(wxMouseEvent& event)
{
    if (HasCapture()) ReleaseMouse();

    if (m_isZoomingArea)
    {
        // Manually normalize the rectangle to ensure width/height are positive.
        // The Normalize() method can be problematic with some compiler setups.
        wxRect screenRect = m_zoomAreaRect;
        if (screenRect.width < 0) {
            screenRect.x += screenRect.width;
            screenRect.width = -screenRect.width;
        }
        if (screenRect.height < 0) {
            screenRect.y += screenRect.height;
            screenRect.height = -screenRect.height;
        }

        if (screenRect.width > 5 && screenRect.height > 5)
        {
            // Convert screen rect to logical rect (in mm * pcb_scale units)
            wxPoint scroll = GetViewStart();
            wxPoint2DDouble logicalTopLeft((screenRect.GetLeft() + scroll.x) / m_scale, (screenRect.GetTop() + scroll.y) / m_scale);
            wxPoint2DDouble logicalBottomRight((screenRect.GetRight() + scroll.x) / m_scale, (screenRect.GetBottom() + scroll.y) / m_scale);
            wxRect2DDouble logicalRect(logicalTopLeft.m_x, logicalTopLeft.m_y, logicalBottomRight.m_x - logicalTopLeft.m_x, logicalBottomRight.m_y - logicalTopLeft.m_y);

            // Zoom to this logical rect
            wxSize clientSize = GetClientSize();
            double scaleX = clientSize.x / logicalRect.m_width;
            double scaleY = clientSize.y / logicalRect.m_height;
            m_scale = std::min(scaleX, scaleY) * 0.95; // 5% padding

            wxPoint2DDouble rectCenter(logicalRect.m_x + logicalRect.m_width / 2.0, logicalRect.m_y + logicalRect.m_height / 2.0);
            wxPoint scrollTarget = wxPoint(rectCenter.m_x * m_scale - clientSize.x / 2,
                                           rectCenter.m_y * m_scale - clientSize.y / 2);
            Scroll(scrollTarget);
        }

        m_isZoomingArea = false;
        m_zoomAreaRect = wxRect();
        SetCursor(wxCURSOR_CROSS);
        Refresh();

        // Notify the parent frame to untoggle the button
        wxCommandEvent zoomCompleteEvent(EVT_ZOOM_AREA_COMPLETE, GetId());
        zoomCompleteEvent.SetEventObject(this);
        wxPostEvent(GetParent(), zoomCompleteEvent);
    }
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

    if (m_isZoomingArea && event.Dragging() && event.LeftIsDown())
    {
        m_zoomAreaRect.SetBottomRight(event.GetPosition());
        Refresh();
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

    // If we are in zoom-to-area mode, cancel it on wheel zoom
    if (m_isZoomingArea) {
        m_isZoomingArea = false;
        SetCursor(wxCURSOR_CROSS);
    }

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