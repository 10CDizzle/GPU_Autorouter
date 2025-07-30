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

RoutingResult AutorouterCore::Route(const RoutingSettings& settings, const wxArrayInt& netsToRoute)
{
    // This is a stub implementation.
    // The actual routing logic will go here.
    RoutingResult result;
    result.success = false; // Not implemented yet
    result.nets_total = netsToRoute.GetCount();
    // Other fields default to 0
    return result;
}