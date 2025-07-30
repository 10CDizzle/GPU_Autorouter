#include "core/PcbParser.h"
#include "core/PcbData.h"
#include <iostream>
#include <stdexcept>

PcbParser::PcbParser() {}

PcbParser::~PcbParser() {}

std::shared_ptr<PcbData> PcbParser::parseFile(const std::string& filePath) {
    if (!m_kicadPcb.loadFromFile(filePath)) {
        std::cerr << "PcbParser failed to load file: " << filePath << std::endl;
        return nullptr;
    }

    std::cout << "File successfully parsed by KicadPcb. Now extracting data..." << std::endl;

    auto pcbData = std::make_shared<PcbData>();
    pcbData->Clear();

    const SexpNode& root = m_kicadPcb.getRootNode();

    // Extract board outline from "Edge.Cuts" layer
    extractBoardOutline(root, *pcbData);

    // TODO: Extract other elements like pads, vias, nets, etc.

    std::cout << "PcbData object created and populated with board outline." << std::endl;

    return pcbData;
}

void PcbParser::extractBoardOutline(const SexpNode& rootNode, PcbData& pcbData) {
    if (!rootNode.isList()) return;

    const auto& rootList = rootNode.getList();
    for (const auto& node : rootList) {
        if (node.isList()) {
            const auto& subList = node.getList();
            if (!subList.empty() && subList[0].isAtom() && subList[0].getAtom() == "gr_line") {
                parseGrLine(node, pcbData);
            }
            // TODO: We could also look for "gr_arc", "gr_circle" etc. on Edge.Cuts here
        }
    }
}

void PcbParser::parseGrLine(const SexpNode& node, PcbData& pcbData) {
    if (!node.isList()) return;

    PcbLine line;
    bool onEdgeCuts = false;

    const auto& properties = node.getList();
    // Skip the first element which is "gr_line"
    for (size_t i = 1; i < properties.size(); ++i) {
        const auto& prop = properties[i];
        if (!prop.isList() || prop.getList().empty() || !prop.getList()[0].isAtom()) {
            continue;
        }

        const auto& propList = prop.getList();
        const std::string& key = propList[0].getAtom();

        try {
            if (key == "start" && propList.size() >= 3) {
                line.start.m_x = std::stod(propList[1].getAtom());
                line.start.m_y = std::stod(propList[2].getAtom());
            } else if (key == "end" && propList.size() >= 3) {
                line.end.m_x = std::stod(propList[1].getAtom());
                line.end.m_y = std::stod(propList[2].getAtom());
            } else if (key == "width" && propList.size() >= 2) {
                line.width = std::stod(propList[1].getAtom());
            } else if (key == "layer" && propList.size() >= 2) {
                line.layer = propList[1].getAtom();
                if (line.layer == "Edge.Cuts") {
                    onEdgeCuts = true;
                }
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Warning: Could not parse number in gr_line: " << e.what() << std::endl;
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Warning: Number out of range in gr_line: " << e.what() << std::endl;
        }
    }

    if (onEdgeCuts) {
        pcbData.AddLine(line);
    }
}
