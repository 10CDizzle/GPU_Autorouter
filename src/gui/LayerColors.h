#ifndef LAYER_COLORS_H
#define LAYER_COLORS_H

#include <wx/colour.h>
#include <wx/string.h>
#include <map>
#include <vector>

struct LayerInfo {
    std::pair<wxColour, wxColour> colours; // Day, Night
    bool visible = true;
};

class LayerColors {
public:
    LayerColors();
    wxColour GetColour(const wxString& layerName, bool nightMode) const;

    void SetVisibility(const wxString& layerName, bool visible);
    bool IsVisible(const wxString& layerName) const;
    void PopulateFromLayers(const std::vector<wxString>& layerNames);

private:
    std::map<wxString, LayerInfo> m_layerInfo;
};

#endif // LAYER_COLORS_H