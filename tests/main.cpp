// This tells Catch2 that we will provide our own main() function.
#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

#include "../src/core/AutorouterCore.h"
#include "../src/core/PcbData.h"
#include <wx/app.h>
#include <wx/filename.h>
#include <wx/dir.h>

// This macro is defined by CMake in tests/CMakeLists.txt
#ifndef PCB_FILES_PATH
    #error "PCB_FILES_PATH is not defined. Check your CMake configuration."
#endif

// We need a dummy wxApp object for the test executable because wxWidgets
// functions (like those used in AutorouterCore) expect one to exist.
class TestApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        // Don't create any windows or run an event loop.
        // Return true to indicate successful initialization.
        return true;
    }
};


// This is the entry point for the test executable.
// We need to initialize wxWidgets before running the tests.
int main(int argc, char* argv[])
{
    // Create a wxWidgets application instance.
    wxApp::SetInstance(new TestApp());

    // wxEntryStart is a platform-specific function that initializes wxWidgets.
    if (!wxEntryStart(argc, argv))
    {
        // Handle initialization failure
        return 1;
    }

    // Run the Catch2 test suite.
    int result = Catch::Session().run(argc, argv);

    // Clean up wxWidgets resources.
    wxEntryCleanup();

    return result;
}

// Helper class to recursively find files
class PcbFileTraverser : public wxDirTraverser
{
public:
    PcbFileTraverser(wxArrayString& files) : m_files(files) {}
    virtual wxDirTraverseResult OnFile(const wxString& filename) override
    {
        m_files.Add(filename);
        return wxDIR_CONTINUE;
    }
    virtual wxDirTraverseResult OnDir(const wxString& dirname) override
    {
        // To make the test suite robust, explicitly ignore version control directories.
        wxFileName fn(dirname);
        if (fn.GetFullName() == ".git" || fn.GetFullName() == ".svn")
        {
            return wxDIR_IGNORE;
        }
        return wxDIR_CONTINUE;
    }
private:
    wxArrayString& m_files;
};


TEST_CASE("PCB File Loading and Routing Metrics", "[core][filesystem]")
{
    // CTest runs executables from the build directory. We need to provide a
    // relative path from the build directory to the source directory where
    // the test files are located. For a typical CMake setup, this is two
    // levels up to the project root, then into `tests/pcb_files`.
    // The path to the test files is passed in by CMake as a compile definition
    // to avoid issues with relative paths and working directories.
    const wxString pcbFilesPath = wxString(PCB_FILES_PATH);

    // Check if the directory exists before trying to traverse it. This helps debug path issues.
    INFO("Searching for PCB files in: " << pcbFilesPath);
    REQUIRE(wxDir::Exists(pcbFilesPath));
    // Discover all .kicad_pcb files in the test directory, recursively.
    wxArrayString pcbFiles;
    PcbFileTraverser traverser(pcbFiles);

    wxDir dir(pcbFilesPath);
    // flag is for newer wxWidgets versions and is not needed here.
    dir.Traverse(traverser, "*.kicad_pcb", wxDIR_FILES | wxDIR_DIRS | wxDIR_HIDDEN);

    // This check is important. If it fails, it means the test runner's
    REQUIRE(pcbFiles.GetCount() > 0);
    INFO("Found " << pcbFiles.GetCount() << " PCB files.");
    INFO("PCB Files:");
    for (const wxString& pcbFile : pcbFiles) {
        INFO("- " << pcbFile);
    }

    

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