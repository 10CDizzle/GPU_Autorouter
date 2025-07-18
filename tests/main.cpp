#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "../src/core/AutorouterCore.h"
#include <wx/app.h>

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

TEST_CASE("PCB File Loading", "[core]")
{
    AutorouterCore core;

    SECTION("Load a simple PCB file") {
        REQUIRE(core.LoadKicadPcb("pcb_files/simple.kicad_pcb"));
    }
}