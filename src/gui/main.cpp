// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/aboutdlg.h>

#include "PcbCanvas.h" // Includes PcbData.h transitively
#include "AutorouterDialog.h"
#include "../core/AutorouterCore.h"
#include <memory>

enum
{
    ID_ToggleNightMode = wxID_HIGHEST + 1,
    ID_OpenRoutingSession,
    ID_Autorouter
};

// Define a new application type, derived from wxApp
class MyApp : public wxApp
{
public:
    // This is the equivalent of main() in a wxWidgets program.
    virtual bool OnInit();

private:
    bool RunHeadlessTest();
};

// Define a new frame type, derived from wxFrame
class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    // A pointer to our custom drawing canvas
    std::unique_ptr<AutorouterCore> m_core;
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
    void OnAutorouter(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

public:
    void SetNightMode(bool nightMode);
};

// The macro that creates the application's main() function.
wxIMPLEMENT_APP(MyApp);

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // Add command line parsing for test mode
    wxCmdLineParser parser(argc, argv);
    parser.AddSwitch("t", "test-mode", "Run in command-line test mode");
    parser.AddOption("pcb", "pcb_file", "Path to KiCad PCB file for testing", wxCMD_LINE_VAL_STRING);

    // We must call the base class method to parse the command line
    if (parser.Parse(false) != 0)
    {
        // This is not a fatal error, just an indication of bad command line
        // in GUI mode. We can ignore it and proceed.
    }

    if (parser.Found("t"))
    {
        // The test runner returns false to prevent the GUI event loop
        return RunHeadlessTest();
    }

    MyFrame *frame = new MyFrame();

    // Check for night mode based on time (e.g., after 6 PM or before 6 AM)
    // wxDateTime now = wxDateTime::Now();
    // int hour = now.GetHour();
    // if (hour >= 18 || hour < 6)
    // {
    //     frame->SetNightMode(true);
    // }

    frame->Show(true);
    return true;
}

bool MyApp::RunHeadlessTest()
{
    wxCmdLineParser parser(argc, argv);
    parser.AddOption("pcb", "pcb_file", "Path to KiCad PCB file for testing", wxCMD_LINE_VAL_STRING);
    parser.Parse();

    wxString pcbFile;
    if (!parser.Found("pcb", &pcbFile)) {
        wxFprintf(stderr, "Error: --pcb argument is required for test mode.\n");
        return false;
    }

    AutorouterCore core;
    if (!core.LoadKicadPcb(pcbFile)) {
        wxFprintf(stderr, "Error: Failed to load PCB file '%s'.\n", pcbFile.c_str());
        return false;
    }

    RoutingSettings settings;
    wxArrayInt netsToRoute; // Route all nets for now
    const auto& allNets = core.GetPcbData().GetNets();
    for (size_t i = 0; i < allNets.size(); ++i) {
        netsToRoute.Add(i);
    }

    RoutingResult result = core.Route(settings, netsToRoute);

    // Print results to stdout in JSON format
    wxPrintf("{\n");
    wxPrintf("  \"success\": %s,\n", result.success ? "true" : "false");
    wxPrintf("  \"routing_time_ms\": %.2f,\n", result.time_ms);
    wxPrintf("  \"nets_total\": %d,\n", result.nets_total);
    wxPrintf("  \"nets_routed\": %d,\n", result.nets_routed);
    if (result.nets_total > 0)
        wxPrintf("  \"completion_rate_pct\": %.2f,\n", (double)result.nets_routed / result.nets_total * 100.0);
    else
        wxPrintf("  \"completion_rate_pct\": 0.0,\n");
    wxPrintf("  \"total_track_length_mm\": %.2f,\n", result.total_track_length);
    wxPrintf("  \"via_count\": %d\n", result.via_count);
    wxPrintf("}\n");

    // returning false from OnInit prevents the main loop
    return false;
}

// MyFrame constructor
MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "PCB Autorouter GUI Test")
{
    m_core = std::make_unique<AutorouterCore>();
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

    wxMenu* menuTools = new wxMenu;
    menuTools->Append(ID_Autorouter, "&Autorouter...\tCtrl-Alt-R");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuView, "&View");
    menuBar->Append(menuTools, "&Tools");
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
    Bind(wxEVT_MENU, &MyFrame::OnAutorouter, this, ID_Autorouter);
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

    if (m_core->LoadKicadPcb(openFileDialog.GetPath()))
    {
        m_canvas->SetPcbData(&m_core->GetPcbData());

        // A new design is loaded, so any previous session file path is invalid
        m_sessionFilePath.clear();
        m_saveMenuItem->Enable(true);
        m_saveAsMenuItem->Enable(true);
        SetTitle(wxString::Format("PCB Autorouter - %s", openFileDialog.GetPath()));
    }
}

void MyFrame::OnOpenRoutingSession(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, "Open routing session file", "", "",
        "PCB Route files (*.pcbroute)|*.pcbroute",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    m_sessionFilePath = openFileDialog.GetPath();
    SessionState state = m_canvas->LoadFile(m_sessionFilePath);

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

    // wxString designPath = openFileDialog.GetPath();
    // m_canvas->LoadSesFile(designPath); // This needs to be moved to core as well

    // // A new design is loaded, so any previous session file path is invalid
    // m_sessionFilePath.clear();
    // m_saveMenuItem->Enable(true);
    // m_saveAsMenuItem->Enable(true);
    // SetTitle(wxString::Format("PCB Autorouter - %s", designPath));
    wxMessageBox("Loading .ses files is not yet implemented in the new core.", "Not Implemented", wxOK | wxICON_INFORMATION, this);
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

void MyFrame::OnAutorouter(wxCommandEvent& event)
{
    const auto& nets = m_core->GetPcbData().GetNets();
    if (nets.empty())
    {
        wxMessageBox("Please open a KiCad PCB file with defined nets first.", "No Nets Loaded", wxOK | wxICON_INFORMATION, this);
        return;
    }

    wxArrayString netNames;
    netNames.reserve(nets.size());
    for(const auto& net : nets) {
        netNames.Add(net);
    }

    AutorouterDialog dlg(this, netNames);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxArrayInt selections = dlg.GetSelectedNets();
        int passes = dlg.GetRoutingPasses();

        RoutingSettings settings{passes};
        m_core->Route(settings, selections); // In a real app, this should be in a thread
        SetStatusText("Routing complete.", 0);
    }
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