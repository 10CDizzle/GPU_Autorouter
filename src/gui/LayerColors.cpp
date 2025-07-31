#include "LayerColors.h"

LayerColors::LayerColors() {
    // Approximations of standard KiCad layer colors
    // Pair is {Day/Bright, Night/Dark}
    m_colours["F.Cu"]       = {wxColour(200, 0, 0),     wxColour(180, 40, 40)};
    m_colours["B.Cu"]       = {wxColour(0, 0, 200),     wxColour(40, 40, 180)};
    m_colours["In1.Cu"]     = {wxColour(200, 128, 0),   wxColour(180, 110, 40)};
    m_colours["In2.Cu"]     = {wxColour(0, 128, 0),     wxColour(40, 110, 40)};
    m_colours["Edge.Cuts"]  = {wxColour(255, 255, 0),   wxColour(220, 220, 80)};
    m_colours["Via"]        = {wxColour(128, 128, 128), wxColour(150, 150, 150)};
    m_colours["Hole"]       = {wxColour(100, 100, 100), wxColour(190, 190, 190)};
}

wxColour LayerColors::GetColour(const wxString& layerName, bool nightMode) const {
    auto it = m_colours.find(layerName);
    if (it != m_colours.end()) {
        // Return the bright or dark variant based on night mode
        return nightMode ? it->second.second : it->second.first;
    }

    // Return a default color for any unhandled layers
    return nightMode ? *wxLIGHT_GREY : *wxBLACK;
}