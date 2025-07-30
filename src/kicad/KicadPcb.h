#ifndef KICAD_PCB_H
#define KICAD_PCB_H

#include <string>
#include "kicad/Sexp.h"

// Forward declarations for KiCad data structures can go here later.

class KicadPcb
{
public:
    KicadPcb();
    ~KicadPcb();

    /**
     * @brief Loads a KiCad PCB file and parses its contents.
     * @param filename The path to the .kicad_pcb file.
     * @return true if loading and parsing was successful, false otherwise.
     */
    bool loadFromFile(const std::string& filename);

    const SexpNode& getRootNode() const;

private:
    SexpNode m_rootNode;

    // We will add member variables to store PCB data here as we parse it.
    // For example:
    // double m_version;
    // std::string m_generator;
};

#endif // KICAD_PCB_H