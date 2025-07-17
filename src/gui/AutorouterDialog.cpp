#include "AutorouterDialog.h"
#include <wx/sizer.h>
#include <wx/stattext.h>

AutorouterDialog::AutorouterDialog(wxWindow* parent, const wxArrayString& netNames)
    : wxDialog(parent, wxID_ANY, "Autorouter Settings", wxDefaultPosition, wxSize(600, 500),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    // --- Main Layout ---
    // Main sizer for the dialog
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Notebook for tabs
    m_notebook = new wxNotebook(this, wxID_ANY);

    // --- General Tab ---
    wxPanel* generalPanel = new wxPanel(m_notebook);
    wxBoxSizer* generalSizer = new wxBoxSizer(wxVERTICAL); // Top-level sizer for this tab
    generalPanel->SetSizerAndFit(generalSizer);

    // --- Routing Target Box ---
    wxStaticBoxSizer* targetSizer = new wxStaticBoxSizer(wxVERTICAL, generalPanel, "Routing Target");
    generalSizer->Add(targetSizer, 1, wxEXPAND | wxALL, 5);

    m_netListBox = new wxCheckListBox(targetSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, netNames);
    targetSizer->Add(m_netListBox, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* selectAllButton = new wxButton(targetSizer->GetStaticBox(), wxID_ANY, "Select All");
    wxButton* selectNoneButton = new wxButton(targetSizer->GetStaticBox(), wxID_ANY, "Select None");
    buttonSizer->Add(selectAllButton, 0, wxRIGHT, 5);
    buttonSizer->Add(selectNoneButton, 0);
    targetSizer->Add(buttonSizer, 0, wxALIGN_LEFT | wxLEFT | wxBOTTOM, 5);

    // --- Routing Settings Box ---
    wxStaticBoxSizer* settingsSizer = new wxStaticBoxSizer(wxVERTICAL, generalPanel, "Routing Settings");
    generalSizer->Add(settingsSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 10, 10); // 2 cols, 10px hgap, 10px vgap
    gridSizer->AddGrowableCol(1, 1);

    gridSizer->Add(new wxStaticText(settingsSizer->GetStaticBox(), wxID_ANY, "Routing Passes:"), 0, wxALIGN_CENTER_VERTICAL);
    m_routingPassesCtrl = new wxSpinCtrl(settingsSizer->GetStaticBox(), wxID_ANY, "10", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100, 10);
    gridSizer->Add(m_routingPassesCtrl, 1, wxEXPAND);
    settingsSizer->Add(gridSizer, 1, wxEXPAND | wxALL, 5);

    generalPanel->SetSizer(generalSizer);
    m_notebook->AddPage(generalPanel, "General");

    // --- Sizes & Clearances Tab ---
    wxPanel* sizesPanel = new wxPanel(m_notebook);
    wxBoxSizer* sizesSizer = new wxBoxSizer(wxVERTICAL);
    sizesSizer->Add(new wxStaticText(sizesPanel, wxID_ANY, "Placeholder for design rules (track widths, clearances, via sizes)."),
                    wxSizerFlags().Border(wxALL, 10));
    sizesPanel->SetSizer(sizesSizer);
    m_notebook->AddPage(sizesPanel, "Sizes & Clearances");

    // --- Layers Tab ---
    wxPanel* layersPanel = new wxPanel(m_notebook);
    wxBoxSizer* layersSizer = new wxBoxSizer(wxVERTICAL);
    layersSizer->Add(new wxStaticText(layersPanel, wxID_ANY, "Placeholder for layer-specific rules (enabled layers, preferred direction)."),
                     wxSizerFlags().Border(wxALL, 10));
    layersPanel->SetSizer(layersSizer);
    m_notebook->AddPage(layersPanel, "Layers");

    // --- Advanced Tab ---
    wxPanel* advancedPanel = new wxPanel(m_notebook);
    wxBoxSizer* advancedSizer = new wxBoxSizer(wxVERTICAL);
    advancedSizer->Add(new wxStaticText(advancedPanel, wxID_ANY, "Placeholder for advanced costing and GPU performance settings."),
                       wxSizerFlags().Border(wxALL, 10));
    advancedPanel->SetSizer(advancedSizer);
    m_notebook->AddPage(advancedPanel, "Advanced");

    mainSizer->Add(m_notebook, 1, wxEXPAND | wxALL, 5);

    // Standard dialog buttons (OK, Cancel)
    mainSizer->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL),
                   wxSizerFlags().Expand().Border(wxALL, 5));

    SetSizerAndFit(mainSizer);
    CentreOnParent();

    // --- Bind Events ---
    selectAllButton->Bind(wxEVT_BUTTON, &AutorouterDialog::OnSelectAll, this);
    selectNoneButton->Bind(wxEVT_BUTTON, &AutorouterDialog::OnSelectNone, this);
}

void AutorouterDialog::OnSelectAll(wxCommandEvent& event)
{
    for (unsigned int i = 0; i < m_netListBox->GetCount(); ++i)
    {
        m_netListBox->Check(i, true);
    }
}

void AutorouterDialog::OnSelectNone(wxCommandEvent& event)
{
    for (unsigned int i = 0; i < m_netListBox->GetCount(); ++i)
    {
        m_netListBox->Check(i, false);
    }
}

wxArrayInt AutorouterDialog::GetSelectedNets() const
{
    wxArrayInt selections;
    m_netListBox->GetCheckedItems(selections);
    return selections;
}

int AutorouterDialog::GetRoutingPasses() const
{
    return m_routingPassesCtrl->GetValue();
}