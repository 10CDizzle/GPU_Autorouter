#include "core/PcbParser.h"
#include "core/PcbData.h"
#include "kicad/KicadPcb.h"
#include "kicad/Sexp.h"
#include <iostream>
#include <stdexcept>

PcbParser::PcbParser() : m_kicadPcb(std::make_unique<KicadPcb>()) {}

PcbParser::~PcbParser() = default;

std::shared_ptr<PcbData> PcbParser::parseFile(const std::string& filePath) {
    if (!m_kicadPcb->loadFromFile(filePath)) {
        std::cerr << "PcbParser failed to load file: " << filePath << std::endl;
        return nullptr;
    }

    std::cout << "File successfully parsed by KicadPcb. Now extracting data..." << std::endl;

    auto pcbData = std::make_shared<PcbData>();
    pcbData->Clear();

    const SexpNode& root = m_kicadPcb->getRootNode();

    // TODO: Implement the data extraction logic here.

    std::cout << "PcbData object created and populated with board outline." << std::endl;

    return pcbData;
}