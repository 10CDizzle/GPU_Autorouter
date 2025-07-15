#include "AutorouterDialog.h"
#include <wx/sizer.h>
#include <wx/stattext.h>

AutorouterDialog::AutorouterDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Autorouter Settings", wxDefaultPosition, wxSize(600, 500),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    // Main sizer for the dialog
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Notebook for tabs
    m_notebook = new wxNotebook(this, wxID_ANY);

    // --- General Tab ---
    wxPanel* generalPanel = new wxPanel(m_notebook);
    wxBoxSizer* generalSizer = new wxBoxSizer(wxVERTICAL);
    generalSizer->Add(new wxStaticText(generalPanel, wxID_ANY, "Placeholder for general routing options (e.g., net selection, routing passes)."),
                      wxSizerFlags().Border(wxALL, 10));
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
}