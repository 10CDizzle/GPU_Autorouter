#include "catch2/catch.hpp"

#include "../src/core/AutorouterCore.h"
#include "../src/core/PcbData.h"
#include <wx/app.h>
#include <wx/filename.h>
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
        wxFileName fn(pcbFile);
        wxString testName;
        wxArrayString dirs = fn.GetDirs();

        // Create a more descriptive name based on the folder structure.
        if (dirs.GetCount() > 1) {
            // It's in a category subdirectory, e.g., pcb_files/high_density_smd/
            // We want a name like "high_density_smd / board.kicad_pcb"
            testName = dirs.Last() + wxT(" / ") + fn.GetFullName();
        } else {
            // It's directly in pcb_files, e.g., pcb_files/simple.kicad_pcb
            testName = fn.GetFullName();
        }

        SECTION(testName.ToStdString())
        {
            AutorouterCore core;
            REQUIRE(core.loadPcbFile(pcbFile.ToStdString()));

            const auto& allNets = core.getPcbData()->GetNets();
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

            // With a real router, we can't know the exact results beforehand.
            // Instead, we check for plausible outcomes.

            // 1. The total number of nets should match the input.
            CHECK(result.nets_total == static_cast<int>(allNets.size()));

            // 2. The number of routed nets should be between 0 and total.
            CHECK(result.nets_routed >= 0);
            CHECK(result.nets_routed <= result.nets_total);

            // 3. Track length must be non-negative. If something was routed, it should be positive.
            CHECK(result.total_track_length >= 0.0);
            if (result.nets_routed > 0) {
                CHECK(result.total_track_length > 0.0);
            }

            // 4. Via count is not implemented yet.
            CHECK(result.via_count == 0);

            // 5. Log the results for manual inspection.
            INFO("Routed " << result.nets_routed << "/" << result.nets_total << " nets in " << result.time_ms << "ms. Total length: " << result.total_track_length << "mm.");
        }
    }
}