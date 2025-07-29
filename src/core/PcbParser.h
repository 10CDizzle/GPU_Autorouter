#ifndef PCB_PARSER_H
#define PCB_PARSER_H

#include "kicad/KicadPcb.h"
#include "kicad/Sexp.h"
#include <memory>
#include <string>

class PcbData; // Forward declaration

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
    KicadPcb m_kicadPcb;

    // Data extraction helpers
    void extractBoardOutline(const SexpNode& rootNode, PcbData& pcbData);
    void parseGrLine(const SexpNode& node, PcbData& pcbData);
};

#endif // PCB_PARSER_H