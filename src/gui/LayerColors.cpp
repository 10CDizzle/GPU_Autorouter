#include "LayerColors.h"
#include <wx/wx.h>

LayerColors::LayerColors() {
    // Approximations of standard KiCad layer colors
    // Pair is {Day/Bright, Night/Dark}
    m_layerInfo["F.Cu"]       = {{wxColour(200, 0, 0),     wxColour(180, 40, 40)}, true};
    m_layerInfo["B.Cu"]       = {{wxColour(0, 0, 200),     wxColour(40, 40, 180)}, true};
    m_layerInfo["In1.Cu"]     = {{wxColour(200, 128, 0),   wxColour(180, 110, 40)}, true};
    m_layerInfo["In2.Cu"]     = {{wxColour(0, 128, 0),     wxColour(40, 110, 40)}, true};
    m_layerInfo["Edge.Cuts"]  = {{wxColour(255, 255, 0),   wxColour(220, 220, 80)}, true};
    m_layerInfo["Via"]        = {{wxColour(128, 128, 128), wxColour(150, 150, 150)}, true};
    m_layerInfo["Hole"]       = {{wxColour(100, 100, 100), wxColour(190, 190, 190)}, true};
}

wxColour LayerColors::GetColour(const wxString& layerName, bool nightMode) const {
    auto it = m_layerInfo.find(layerName);
    if (it != m_layerInfo.end()) {
        // Return the bright or dark variant based on night mode
        return nightMode ? it->second.colours.second : it->second.colours.first;
    }

    // Return a default color for any unhandled layers
    return nightMode ? *wxLIGHT_GREY : *wxBLACK;
}

void LayerColors::SetVisibility(const wxString& layerName, bool visible) {
    auto it = m_layerInfo.find(layerName);
    if (it != m_layerInfo.end()) {
        it->second.visible = visible;
    }
}

bool LayerColors::IsVisible(const wxString& layerName) const {
    auto it = m_layerInfo.find(layerName);
    if (it != m_layerInfo.end()) {
        return it->second.visible;
    }
    // If a layer isn't in our map, it's not visible by default.
    return false;
}

void LayerColors::PopulateFromLayers(const std::vector<wxString>& layerNames) {
    // This ensures that any layer present in the PCB but not in our
    // hardcoded list gets an entry, so it can be made visible.
    for (const auto& name : layerNames) {
        if (m_layerInfo.find(name) == m_layerInfo.end()) {
            // Add with a default color (magenta for day, cyan for night) and visible.
            m_layerInfo[name] = {{wxColour(255, 0, 255), wxColour(0, 255, 255)}, true};
        }
    }
}