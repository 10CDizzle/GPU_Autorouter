#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/checklst.h>
#include <wx/spinctrl.h>

class AutorouterDialog : public wxDialog
{
public:
    AutorouterDialog(wxWindow* parent, const wxArrayString& netNames);

    // --- Getters for user settings ---
    wxArrayInt GetSelectedNets() const;
    int GetRoutingPasses() const;

private:
    // --- Event Handlers ---
    void OnSelectAll(wxCommandEvent& event);
    void OnSelectNone(wxCommandEvent& event);

    wxNotebook* m_notebook;
    wxCheckListBox* m_netListBox;
    wxSpinCtrl* m_routingPassesCtrl;
};