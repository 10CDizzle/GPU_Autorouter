#include "core/AutorouterCore.h"
#include "core/PcbData.h"
#include "core/PcbParser.h"

AutorouterCore::AutorouterCore() : m_parser(std::make_unique<PcbParser>()) {}

AutorouterCore::~AutorouterCore() {}

bool AutorouterCore::loadPcbFile(const std::string& filePath) {
    m_pcbData = m_parser->parseFile(filePath);
    return m_pcbData != nullptr;
}

std::shared_ptr<PcbData> AutorouterCore::getPcbData() const {
    return m_pcbData;
}