#include "catch2/catch.hpp"

#include "../src/core/AutorouterCore.h"
#include <wx/app.h>
#include <wx/dir.h>

// We need a dummy wxApp object for the test executable because wxWidgets
// functions (like those used in AutorouterCore) expect one to exist.
class TestApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        // Don't create any windows or run an event loop
        return false;
    }
};

IMPLEMENT_APP_NO_MAIN(TestApp);

TEST_CASE("PCB File Loading and Routing Metrics", "[core][filesystem]")
{
    // Discover all .kicad_pcb files in the test directory.
    // This assumes the test is run from the `tests` directory, which is
    // configured in .vscode/settings.json.
    wxArrayString pcbFiles;
    wxDir::GetAllFiles("pcb_files", &pcbFiles, "*.kicad_pcb", wxDIR_FILES);

    // This check is important. If it fails, it means the test runner's
    // working directory is wrong or the pcb_files folder is empty.
    INFO("Make sure the 'pcb_files' directory exists in the 'tests' folder and contains .kicad_pcb files.");
    REQUIRE(pcbFiles.GetCount() > 0);

    // This loop creates a dynamic section for each file found.
    for (const wxString& pcbFile : pcbFiles)
    {
        // Use the filename as the section name for clear test reporting.
        SECTION(pcbFile.ToStdString())
        {
            AutorouterCore core;
            REQUIRE(core.LoadKicadPcb(pcbFile));

            const auto& allNets = core.GetPcbData().GetNets();
            if (allNets.empty()) {
                // It's valid for a PCB to have no nets. We can't test routing,
                // but successfully loading it is a pass.
                SUCCEED("Successfully loaded PCB with no nets.");
                continue; // Skip to the next file.
            }

            wxArrayInt netsToRoute;
            for (size_t i = 0; i < allNets.size(); ++i) {
                netsToRoute.Add(i);
            }

            RoutingSettings settings;
            RoutingResult result = core.Route(settings, netsToRoute);

            // Check against the placeholder values currently in AutorouterCore::Route.
            CHECK(result.nets_total == static_cast<int>(allNets.size()));
            CHECK(result.nets_routed == static_cast<int>(allNets.size() * 0.95));
            CHECK(result.total_track_length == 1234.5);
            CHECK(result.via_count == 87);
        }
    }
}