#ifndef AUTOROUTER_CORE_H
#define AUTOROUTER_CORE_H

#include <memory>
#include <string>

// For wxArrayInt
#include <wx/dynarray.h>
int WX_DECLARE_ARRAY_INT(int, wxArrayInt);

class PcbData;
class PcbParser;

struct RoutingSettings {
    int routing_passes = 10;
    // Other settings like track width, clearance, etc. can be added here.
};

struct RoutingResult {
    bool success = false;
    double time_ms = 0.0;
    int nets_total = 0;
    int nets_routed = 0;
    double total_track_length = 0.0;
    int via_count = 0;
};

class AutorouterCore {
public:
    AutorouterCore();
    ~AutorouterCore();

    /**
     * @brief Loads a PCB from a file.
     * @param filePath The path to the PCB file.
     * @return true on success, false on failure.
     */
    bool loadPcbFile(const std::string& filePath);

    std::shared_ptr<PcbData> getPcbData() const;

    RoutingResult Route(const RoutingSettings& settings, const wxArrayInt& netsToRoute);

private:
    std::unique_ptr<PcbParser> m_parser;
    std::shared_ptr<PcbData> m_pcbData;
};

#endif // AUTOROUTER_CORE_H