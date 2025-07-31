#include "LayerControlPanel.h"
#include <wx/sizer.h>
#include <wx/statbox.h>

// Define the custom event type
wxDEFINE_EVENT(EVT_LAYER_VISIBILITY_CHANGED, wxCommandEvent);

LayerControlPanel::LayerControlPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    wxStaticBoxSizer* sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Layers");
    m_checkListBox = new wxCheckListBox(this, wxID_ANY);
    sizer->Add(m_checkListBox, 1, wxEXPAND | wxALL, 5);

    SetSizerAndFit(sizer);

    m_checkListBox->Bind(wxEVT_CHECKLISTBOX, &LayerControlPanel::OnCheckListBox, this);
}

void LayerControlPanel::PopulateLayers(const std::vector<wxString>& layerNames)
{
    m_checkListBox->Clear();
    for (const auto& name : layerNames)
    {
        int index = m_checkListBox->Append(name);
        m_checkListBox->Check(index, true);
    }
}

void LayerControlPanel::OnCheckListBox(wxCommandEvent& event)
{
    int itemIndex = event.GetInt();
    bool isChecked = m_checkListBox->IsChecked(itemIndex);
    wxString layerName = m_checkListBox->GetString(itemIndex);

    // Create a new event to notify the parent frame
    wxCommandEvent newEvent(EVT_LAYER_VISIBILITY_CHANGED, GetId());
    newEvent.SetEventObject(this);
    newEvent.SetString(layerName);
    newEvent.SetInt(isChecked);

    // Send the event to the parent
    wxPostEvent(GetParent(), newEvent);
}