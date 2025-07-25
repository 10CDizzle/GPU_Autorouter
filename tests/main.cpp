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

TEST_CASE("Routing Metrics Evaluation", "[core]")
{
    AutorouterCore core;
    REQUIRE(core.LoadKicadPcb("pcb_files/simple.kicad_pcb"));

    // Get the list of nets from the loaded PCB
    const auto& allNets = core.GetPcbData().GetNets();
    REQUIRE(allNets.size() > 0); // Ensure the test file has nets

    // For this test, we'll "route" all of them.
    wxArrayInt netsToRoute;
    for (size_t i = 0; i < allNets.size(); ++i) {
        netsToRoute.Add(i);
    }

    RoutingSettings settings;
    RoutingResult result = core.Route(settings, netsToRoute);

    // Check against the placeholder values currently in AutorouterCore::Route.
    // When the real router is implemented, these expected values will change,
    // and this test will act as a regression guard.
    CHECK(result.nets_total == static_cast<int>(allNets.size()));
    CHECK(result.nets_routed == static_cast<int>(allNets.size() * 0.95));
    CHECK(result.total_track_length == 1234.5);
    CHECK(result.via_count == 87);
}