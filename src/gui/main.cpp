// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// Define a new application type, derived from wxApp
class MyApp : public wxApp
{
public:
    // This is the equivalent of main() in a wxWidgets program.
    virtual bool OnInit();
};

// Define a new frame type, derived from wxFrame
class MyFrame : public wxFrame
{
public:
    MyFrame();
};

// The macro that creates the application's main() function.
wxIMPLEMENT_APP(MyApp);

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}

// MyFrame constructor
MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "PCB Autorouter GUI Test")
{
    // Set a minimum size for the window
    SetMinSize(wxSize(400, 300));

    // Create a panel to hold the controls
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Create the text label
    wxStaticText* label = new wxStaticText(panel, wxID_ANY, "Hello, CUDA PCB Autorouter!");

    // Use a sizer to manage the layout and center the label
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(label, 1, wxEXPAND | wxALL, 20); // Add with border and make it expand
    panel->SetSizerAndFit(sizer);

    // Center the frame on the screen
    Centre();
}