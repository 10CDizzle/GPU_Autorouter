#include "PcbPanel.h"
#include <wx/dcbuffer.h>
#include <algorithm> // For std::min
#include <cmath>     // For round

wxBEGIN_EVENT_TABLE(PcbPanel, wxScrolledWindow)
    EVT_PAINT(PcbPanel::OnPaint)
    EVT_SIZE(PcbPanel::OnSize)
    EVT_LEFT_DOWN(PcbPanel::OnMouseDown)
    EVT_LEFT_UP(PcbPanel::OnMouseUp)
    EVT_MOTION(PcbPanel::OnMouseMove)
    EVT_MOUSEWHEEL(PcbPanel::OnMouseWheel)
wxEND_EVENT_TABLE()
// Constructor
PcbPanel::PcbPanel(wxWindow* parent)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL | wxFULL_REPAINT_ON_RESIZE)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT); // Use custom paint handler to avoid flicker
    SetNightMode(false); // Initialize with light mode colors

    // Create buttons
    wxButton* zoomInButton = new wxButton(this, wxID_ANY, "Zoom In");
    wxButton* zoomOutButton = new wxButton(this, wxID_ANY, "Zoom Out");

    // Bind button events (replace with your actual zoom functions)
    zoomInButton->Bind(wxEVT_BUTTON, &PcbPanel::OnZoomInClicked, this);
    zoomOutButton->Bind(wxEVT_BUTTON, &PcbPanel::OnZoomOutClicked, this);

    // Use a sizer to manage button layout
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(zoomInButton, 0, wxALL | wxCENTER, 5);
    buttonSizer->Add(zoomOutButton, 0, wxALL | wxCENTER, 5);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, 5);

    SetSizer(mainSizer);
    mainSizer->SetSizeHints(this);
}

void PcbPanel::SetPcbData(const PcbData* data)
{
    m_pcbData = data;
    ZoomToFit();
    Refresh();
}

void PcbPanel::SetNightMode(bool nightMode)
{
    if (nightMode)
    {
        m_bgColour = wxColour(30, 30, 30);
        m_boardOutlineColour = wxColour(255, 255, 0); // Yellow
        m_padColourTop = wxColour(200, 0, 0); // Red
        m_padColourBottom = wxColour(0, 0, 200); // Blue
        m_textColour = wxColour(220, 220, 220);
    }
    else
    {
        m_bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
        m_boardOutlineColour = wxColour(0, 0, 0); // Black
        m_padColourTop = wxColour(200, 0, 0); // Red
        m_padColourBottom = wxColour(0, 0, 200); // Blue
        m_textColour = *wxBLACK;
    }
    SetBackgroundColour(m_bgColour);
    Refresh();
}
void PcbPanel::ZoomIn()
{
    m_scale *= 1.2;
    Refresh();
}

void PcbPanel::ZoomOut()
{
    m_scale /= 1.2;
    Refresh();
}

void PcbPanel::ZoomToFit()
{
    if (!m_pcbData || m_pcbData->GetBoundingBox().IsEmpty())
    {
        m_scale = 1.0;
        m_panOffset = wxPoint2DDouble(0.0, 0.0);
        Refresh();
        return;
    }

    wxRect2DDouble bbox = m_pcbData->GetBoundingBox();
    wxSize clientSize = GetClientSize();

    if (bbox.GetWidth() <= 0 || bbox.GetHeight() <= 0 || clientSize.x <= 0 || clientSize.y <= 0)
    {
        return;
    }

    // Add some padding
    const double padding = 0.9;
    double scaleX = (clientSize.x * padding) / bbox.GetWidth();
    double scaleY = (clientSize.y * padding) / bbox.GetHeight();

    m_scale = std::min(scaleX, scaleY);

    // Center the view on the bounding box's center
    m_panOffset.m_x = bbox.GetLeft() + (bbox.GetWidth() / 2.0);
    m_panOffset.m_y = bbox.GetTop() + (bbox.GetHeight() / 2.0);

    Refresh();
}

void PcbPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    if (m_pcbData)
    {
        DrawPcb(dc);
    }
    else
    {
        // Draw a placeholder message
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();
        dc.SetFont(*wxNORMAL_FONT);
        dc.DrawLabel("No PCB loaded.", GetClientRect(), wxALIGN_CENTER);
    }
}

void PcbPanel::OnSize(wxSizeEvent& event)
{
    // When the panel is resized, re-zoom to fit to keep the board centered and scaled.
    ZoomToFit();
    event.Skip();
}

void PcbPanel::OnMouseDown(wxMouseEvent& event)
{
    if (event.LeftDown())
    {
        m_isPanning = true;
        m_panStartPos = event.GetPosition();
        SetCursor(wxCursor(wxCURSOR_HAND));
    }
}

void PcbPanel::OnMouseUp(wxMouseEvent& event)
{
    if (event.LeftUp())
    {
        m_isPanning = false;
        SetCursor(wxCursor(wxCURSOR_ARROW));
    }
}

void PcbPanel::OnMouseMove(wxMouseEvent& event)
{
    if (m_isPanning && event.Dragging())
    {
        wxPoint currentPos = event.GetPosition();
        wxPoint delta = currentPos - m_panStartPos;

        m_panOffset.m_x -= delta.x / m_scale;
        m_panOffset.m_y += delta.y / m_scale; // Y is inverted in screen coords

        m_panStartPos = currentPos;
        Refresh();
    }
}

void PcbPanel::OnMouseWheel(wxMouseEvent& event)
{
    if (event.GetWheelRotation() > 0)
    {
        ZoomIn();
    }
    else{
        ZoomOut();
    }
}

void PcbPanel::OnZoomInClicked(wxCommandEvent& event)
{
    ZoomIn();
}
}
    else