#ifndef LAYER_COLORS_H
#define LAYER_COLORS_H

#include <wx/colour.h>
#include <wx/string.h>
#include <map>

class LayerColors {
public:
    LayerColors();
    wxColour GetColour(const wxString& layerName, bool nightMode) const;

private:
    std::map<wxString, std::pair<wxColour, wxColour>> m_colours;
};

#endif // LAYER_COLORS_H