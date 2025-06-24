// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "PcbCanvas.h" // Include our new custom canvas

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

private:
    // A pointer to our custom drawing canvas
    PcbCanvas* m_canvas;

    // Event handlers
    void OnOpenKicad(wxCommandEvent& event);
    void OnOpenSes(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
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
    // --- 1. Create the Menubar ---
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_OPEN, "&Open KiCad PCB...\tCtrl-O");
    menuFile->Append(wxID_ADD, "Open &SES File..."); // Using a different ID for a separate handler
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT); // Use a standard ID for the Exit item

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    // --- 2. Create the Status Bar ---
    CreateStatusBar();
    SetStatusText("Welcome to the GPU PCB Autorouter!");

    // --- 3. Create the Central Drawing Area ---
    // This panel will eventually be a custom widget for displaying the PCB.
    m_canvas = new PcbCanvas(this);

    // --- 4. Use a Sizer to Arrange Components ---
    // The sizer will manage the layout, allowing the drawingCanvas to expand
    // and fill the available space in the frame.
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_canvas, 1, wxEXPAND); // The '1' makes it the expanding "stretchable" part

    this->SetSizerAndFit(sizer);
    this->SetMinSize(wxSize(800, 600)); // Set a more reasonable default size

    // Center the frame on the screen
    Centre();

    // --- 5. Connect Events to Handlers ---
    Bind(wxEVT_MENU, &MyFrame::OnOpenKicad, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MyFrame::OnOpenSes, this, wxID_ADD);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::OnOpenKicad(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, "Open KiCad PCB file", "", "",
                               "KiCad PCB files (*.kicad_pcb)|*.kicad_pcb",
                               wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // The user cancelled

    m_canvas->LoadKicadPcb(openFileDialog.GetPath());
}

void MyFrame::OnOpenSes(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, "Open SES file", "", "",
                               "Spectra Session files (*.ses)|*.ses",
                               wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    m_canvas->LoadSesFile(openFileDialog.GetPath());
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a GPU-accelerated PCB Autorouter application.",
                 "About PCB Autorouter", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}