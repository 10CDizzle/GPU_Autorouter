// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/aboutdlg.h>

#include "PcbCanvas.h" // Include our new custom canvas

enum
{
    ID_ToggleNightMode = wxID_HIGHEST + 1,
    ID_OpenRoutingSession
};

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
    bool m_isNightMode;

    // Path for the current session file
    wxString m_sessionFilePath;
    wxMenuItem* m_saveMenuItem;
    wxMenuItem* m_saveAsMenuItem;

    // Event handlers
    void OnOpenKicad(wxCommandEvent& event);
    void OnOpenRoutingSession(wxCommandEvent& event);
    void OnOpenSes(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnToggleNightMode(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

public:
    void SetNightMode(bool nightMode);
};

// The macro that creates the application's main() function.
wxIMPLEMENT_APP(MyApp);

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();

    // Check for night mode based on time (e.g., after 6 PM or before 6 AM)
    wxDateTime now = wxDateTime::Now();
    int hour = now.GetHour();
    if (hour >= 18 || hour < 6)
    {
        frame->SetNightMode(true);
    }

    frame->Show(true);
    return true;
}

// MyFrame constructor
MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "PCB Autorouter GUI Test")
{
    m_isNightMode = false; // Default to light mode

    // --- 1. Create the Menubar ---
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_OPEN, "&Open KiCad PCB...\tCtrl-O");
    menuFile->Append(wxID_ADD, "Open &SES File..."); // Using a different ID for a separate handler
    menuFile->Append(ID_OpenRoutingSession, "Open &Routing Session...\tCtrl-R");
    menuFile->AppendSeparator();
    m_saveMenuItem = menuFile->Append(wxID_SAVE, "&Save\tCtrl-S");
    m_saveAsMenuItem = menuFile->Append(wxID_SAVEAS, "Save &As...");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT); // Use a standard ID for the Exit item

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenu* menuView = new wxMenu;
    menuView->AppendCheckItem(ID_ToggleNightMode, "&Night Mode\tCtrl-N");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuView, "&View");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    m_saveMenuItem->Enable(false);
    m_saveAsMenuItem->Enable(false);

    // --- 2. Create the Status Bar ---
    CreateStatusBar(2); // Create a status bar with 2 fields
    SetStatusText("Welcome to the GPU PCB Autorouter!", 0);

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
    Bind(wxEVT_MENU, &MyFrame::OnOpenRoutingSession, this, ID_OpenRoutingSession);
    Bind(wxEVT_MENU, &MyFrame::OnOpenSes, this, wxID_ADD);
    Bind(wxEVT_MENU, &MyFrame::OnSave, this, wxID_SAVE);
    Bind(wxEVT_MENU, &MyFrame::OnSaveAs, this, wxID_SAVEAS);
    Bind(wxEVT_MENU, &MyFrame::OnToggleNightMode, this, ID_ToggleNightMode);
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

    wxString designPath = openFileDialog.GetPath();
    m_canvas->LoadKicadPcb(designPath);

    // A new design is loaded, so any previous session file path is invalid
    m_sessionFilePath.clear();
    m_saveMenuItem->Enable(true);
    m_saveAsMenuItem->Enable(true);
    SetTitle(wxString::Format("PCB Autorouter - %s", designPath));
}

void MyFrame::OnOpenRoutingSession(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, "Open routing session file", "", "",
        "PCB Route files (*.pcbroute)|*.pcbroute",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    m_sessionFilePath = openFileDialog.GetPath();
    PcbCanvas::SessionState state = m_canvas->LoadFile(m_sessionFilePath);

    if (state.loaded)
    {
        m_canvas->ApplySessionState(state);
        SetNightMode(state.isNightMode); // Apply night mode from session
        SetTitle(wxString::Format("PCB Autorouter - %s", m_sessionFilePath));
        m_saveMenuItem->Enable(true);
        m_saveAsMenuItem->Enable(true);
    }
}

void MyFrame::OnOpenSes(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, "Open SES file", "", "",
                               "Spectra Session files (*.ses)|*.ses",
                               wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString designPath = openFileDialog.GetPath();
    m_canvas->LoadSesFile(designPath);

    // A new design is loaded, so any previous session file path is invalid
    m_sessionFilePath.clear();
    m_saveMenuItem->Enable(true);
    m_saveAsMenuItem->Enable(true);
    SetTitle(wxString::Format("PCB Autorouter - %s", designPath));
}

void MyFrame::OnSave(wxCommandEvent& event)
{
    if (m_sessionFilePath.IsEmpty())
    {
        // If no path is set, behave like "Save As..."
        OnSaveAs(event);
    }
    else
    {
        m_canvas->SaveFile(m_sessionFilePath);
    }
}

void MyFrame::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog saveFileDialog(this, "Save routing session file", "", "",
                               "PCB Route files (*.pcbroute)|*.pcbroute",
                               wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;

    m_sessionFilePath = saveFileDialog.GetPath();
    m_canvas->SaveFile(m_sessionFilePath);
    SetTitle(wxString::Format("PCB Autorouter - %s", m_sessionFilePath));
}

void MyFrame::OnToggleNightMode(wxCommandEvent& event)
{
    SetNightMode(event.IsChecked());
}

void MyFrame::SetNightMode(bool nightMode)
{
    m_isNightMode = nightMode;
    GetMenuBar()->Check(ID_ToggleNightMode, m_isNightMode);

    wxColour bgColour = nightMode ? wxColour(30, 30, 30) : wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
    wxColour fgColour = nightMode ? wxColour(220, 220, 220) : wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);

    // Update frame itself
    SetBackgroundColour(bgColour);

    // Update canvas
    if (m_canvas)
    {
        m_canvas->SetNightMode(nightMode);
    }

    // Update status bar
    wxStatusBar* statusBar = GetStatusBar();
    if (statusBar)
    {
        statusBar->SetBackgroundColour(bgColour);
        statusBar->SetForegroundColour(fgColour);
    }

    Refresh();
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("GPU PCB Autorouter");
    aboutInfo.SetVersion("0.1.0");
    aboutInfo.SetDescription(_("A hardware-accelerated autorouter for printed circuit boards."));
    aboutInfo.SetCopyright("(C) 2024 Chris");
    aboutInfo.SetWebSite("https://github.com/Chris-plus-plus/GPU_Autorouter", "Project GitHub Page");
    aboutInfo.AddDeveloper("Chris");
    // You can add more info like license, etc.

    wxAboutBox(aboutInfo, this);
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}