#ifndef PCB_PARSER_H
#define PCB_PARSER_H

#include <memory>
#include <string>

class PcbData; // Forward declaration
class KicadPcb; // Forward declaration

class PcbParser {
public:
    PcbParser();
    ~PcbParser();

    /**
     * @brief Loads and parses a KiCad PCB file.
     * @param filePath The path to the .kicad_pcb file.
     * @return A shared pointer to the populated PcbData object, or nullptr on failure.
     */
    std::shared_ptr<PcbData> parseFile(const std::string& filePath);

private:
    std::unique_ptr<KicadPcb> m_kicadPcb;
};

#endif // PCB_PARSER_H
