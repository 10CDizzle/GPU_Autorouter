#ifndef LAYER_CONTROL_PANEL_H
#define LAYER_CONTROL_PANEL_H

#include <wx/panel.h>
#include <wx/checklst.h>
#include <vector>

// Define a custom event for when layer visibility changes.
wxDECLARE_EVENT(EVT_LAYER_VISIBILITY_CHANGED, wxCommandEvent);

class LayerControlPanel : public wxPanel
{
public:
    LayerControlPanel(wxWindow* parent);

    void PopulateLayers(const std::vector<wxString>& layerNames);

private:
    void OnCheckListBox(wxCommandEvent& event);

    wxCheckListBox* m_checkListBox;
};

#endif // LAYER_CONTROL_PANEL_H