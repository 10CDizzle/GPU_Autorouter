#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>

class AutorouterDialog : public wxDialog
{
public:
    AutorouterDialog(wxWindow* parent);

private:
    wxNotebook* m_notebook;
};