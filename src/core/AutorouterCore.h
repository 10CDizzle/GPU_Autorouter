#pragma once

#include "PcbData.h"
#include "RoutingGrid.h"
#include <wx/stopwatch.h>

struct RoutingSettings
{
    int passes = 10;
    // Future settings: track_width, clearance, via_cost, etc.
};

struct RoutingResult
{
    bool success = false;
    double time_ms = 0.0;
    int nets_total = 0;
    int nets_routed = 0;
    double total_track_length = 0.0;
    int via_count = 0;
};

class AutorouterCore
{
public:
    AutorouterCore();

    bool LoadKicadPcb(const wxString& path);

    const PcbData& GetPcbData() const { return m_pcbData; }

    RoutingResult Route(const RoutingSettings& settings, const wxArrayInt& netsToRoute);

private:
    // Placeholder for the actual GPU routing engine
    void RunGpuRouting(const RoutingSettings& settings);

    PcbData m_pcbData;
};