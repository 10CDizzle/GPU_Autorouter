#include "core/PcbParser.h"
#include "core/PcbData.h"
#include "kicad/KicadPcb.h"
#include "kicad/Sexp.h"
#include <iostream>
#include <wx/log.h>
#include <stdexcept>

PcbParser::PcbParser() : m_kicadPcb(std::make_unique<KicadPcb>()) {}

PcbParser::~PcbParser() = default;

namespace {
    // Helper to find a child node by its key, e.g., find (layer F.Cu) within a parent.
    const SexpNode* findNode(const SexpNode& parent, const std::string& key) {
        if (!parent.isList()) {
            return nullptr;
        }
        for (const auto& child : parent.getList()) {
            if (child.isList() && !child.getList().empty() && child.getList()[0].isAtom() && child.getList()[0].getAtom() == key) {
                return &child;
            }
        }
        return nullptr;
    }

    // Helper to parse nodes like (at x y), (start x y), (end x y)
    bool parsePoint(const SexpNode* node, wxPoint2DDouble& point) {
        if (!node || !node->isList() || node->getList().size() < 3) return false;
        try {
            point.m_x = std::stod(node->getList()[1].getAtom());
            point.m_y = std::stod(node->getList()[2].getAtom());
            return true;
        } catch (const std::invalid_argument& e) {
            wxLogError("Could not parse point coordinate: %s", e.what());
            return false;
        }
    }

    // Helper to parse nodes like (size w h)
    bool parseSize(const SexpNode* node, wxPoint2DDouble& size) {
        if (!node || !node->isList() || node->getList().size() < 3) return false;
        try {
            size.m_x = std::stod(node->getList()[1].getAtom());
            size.m_y = std::stod(node->getList()[2].getAtom());
            return true;
        } catch (const std::invalid_argument& e) {
            wxLogError("Could not parse size coordinate: %s", e.what());
            return false;
        }
    }

    // --- Individual element parsers ---

    void parseNet(const SexpNode& node, PcbData& pcbData) {
        if (node.getList().size() > 2 && node.getList()[2].isAtom()) {
            pcbData.AddNet(node.getList()[2].getAtom());
        }
    }

    void parseGrLine(const SexpNode& node, PcbData& pcbData) {
        const SexpNode* layerNode = findNode(node, "layer");
        // Only care about Edge.Cuts for gr_line
        if (layerNode && layerNode->getList().size() > 1 && layerNode->getList()[1].getAtom() == "Edge.Cuts") {
            PcbLine line;
            const SexpNode* startNode = findNode(node, "start");
            const SexpNode* endNode = findNode(node, "end");
            const SexpNode* widthNode = findNode(node, "width");

            if (parsePoint(startNode, line.start) &&
                parsePoint(endNode, line.end) &&
                widthNode && widthNode->getList().size() > 1) {
                line.width = std::stod(widthNode->getList()[1].getAtom());
                line.layer = "Edge.Cuts";
                pcbData.AddLine(line);
            }
        }
    }

    void parsePad(const SexpNode& node, PcbData& pcbData) {
        if (node.getList().size() < 4) return; // Not a valid pad definition

        PcbPad pad;
        const SexpNode* atNode = findNode(node, "at");
        const SexpNode* sizeNode = findNode(node, "size");
        const SexpNode* layersNode = findNode(node, "layers");
        const SexpNode* netNode = findNode(node, "net");

        const std::string& padType = node.getList()[2].getAtom();
        if (padType == "np_thru_hole") {
            pad.shape = "np_thru_hole"; // Use this special value for the renderer
            if (parsePoint(atNode, pad.pos) && parseSize(sizeNode, pad.size)) {
                pcbData.AddPad(pad);
            }
            return;
        }

        // For normal pads (smd, thru_hole)
        if (parsePoint(atNode, pad.pos) && parseSize(sizeNode, pad.size) &&
            node.getList()[3].isAtom() &&
            layersNode && layersNode->getList().size() > 1 &&
            netNode && netNode->getList().size() > 1) {
            
            pad.shape = node.getList()[3].getAtom(); // Shape is the 4th element
            pad.layer = layersNode->getList()[1].getAtom();
            pad.netId = std::stoi(netNode->getList()[1].getAtom());
            pcbData.AddPad(pad);
        }
    }

    void parseSegment(const SexpNode& node, PcbData& pcbData) {
        const SexpNode* layerNode = findNode(node, "layer");
        if (layerNode && layerNode->getList().size() > 1) {
            PcbLine segment;
            const SexpNode* startNode = findNode(node, "start");
            const SexpNode* endNode = findNode(node, "end");
            const SexpNode* widthNode = findNode(node, "width");
            const SexpNode* netNode = findNode(node, "net");

            if (parsePoint(startNode, segment.start) &&
                parsePoint(endNode, segment.end) &&
                widthNode && widthNode->getList().size() > 1 &&
                netNode && netNode->getList().size() > 1) {
                segment.width = std::stod(widthNode->getList()[1].getAtom());
                segment.layer = layerNode->getList()[1].getAtom();
                segment.netId = std::stoi(netNode->getList()[1].getAtom());
                pcbData.AddLine(segment);
            }
        }
    }

    void parseVia(const SexpNode& node, PcbData& pcbData) {
        PcbVia via;
        const SexpNode* atNode = findNode(node, "at");
        const SexpNode* sizeNode = findNode(node, "size");
        const SexpNode* drillNode = findNode(node, "drill");
        const SexpNode* layersNode = findNode(node, "layers");
        const SexpNode* netNode = findNode(node, "net");

        if (parsePoint(atNode, via.pos) &&
            sizeNode && sizeNode->getList().size() > 1 &&
            drillNode && drillNode->getList().size() > 1 &&
            layersNode && layersNode->getList().size() > 2 &&
            netNode && netNode->getList().size() > 1) {
            via.size = std::stod(sizeNode->getList()[1].getAtom());
            via.drill = std::stod(drillNode->getList()[1].getAtom());
            via.fromLayer = layersNode->getList()[1].getAtom();
            via.toLayer = layersNode->getList()[2].getAtom();
            via.netId = std::stoi(netNode->getList()[1].getAtom());
            pcbData.AddVia(via);
        }
    }

    void parseZone(const SexpNode& node, PcbData& pcbData) {
        PcbZone zone;
        const SexpNode* layerNode = findNode(node, "layer");
        const SexpNode* netNode = findNode(node, "net");
        const SexpNode* polygonNode = findNode(node, "polygon");

        if (layerNode && layerNode->getList().size() > 1 && netNode && netNode->getList().size() > 1 && polygonNode) {
            zone.layer = layerNode->getList()[1].getAtom();
            zone.netId = std::stoi(netNode->getList()[1].getAtom());
            const SexpNode* ptsNode = findNode(*polygonNode, "pts");
            if (ptsNode && ptsNode->isList()) {
                for (const auto& ptNode : ptsNode->getList()) {
                    if (ptNode.isList() && !ptNode.getList().empty() && ptNode.getList()[0].getAtom() == "xy" && ptNode.getList().size() > 2) {
                        zone.polygon.emplace_back(std::stod(ptNode.getList()[1].getAtom()), std::stod(ptNode.getList()[2].getAtom()));
                    }
                }
            }
            if (!zone.polygon.empty()) pcbData.AddZone(zone);
        }
    }

    // --- The recursive traversal function ---
    void recursiveExtract(const SexpNode& node, PcbData& pcbData) {
        if (!node.isList() || node.getList().empty()) {
            return;
        }

        // Check the type of the current node
        if (node.getList()[0].isAtom()) {
            const std::string& nodeType = node.getList()[0].getAtom();
            if (nodeType == "net") {
                parseNet(node, pcbData);
            } else if (nodeType == "gr_line") {
                parseGrLine(node, pcbData);
            } else if (nodeType == "pad") {
                parsePad(node, pcbData);
            } else if (nodeType == "segment") {
                parseSegment(node, pcbData);
            } else if (nodeType == "via") {
                parseVia(node, pcbData);
            } else if (nodeType == "zone") {
                parseZone(node, pcbData);
            }
        }

        // Recurse into children
        for (const auto& child : node.getList()) {
            recursiveExtract(child, pcbData);
        }
    }
} // anonymous namespace

std::shared_ptr<PcbData> PcbParser::parseFile(const std::string& filePath) {
    if (!m_kicadPcb->loadFromFile(filePath)) {
        std::cerr << "PcbParser failed to load file: " << filePath << std::endl;
        return nullptr;
    }

    std::cout << "File successfully parsed by KicadPcb. Now extracting data..." << std::endl;

    auto pcbData = std::make_shared<PcbData>();
    pcbData->Clear();

    const SexpNode& root = m_kicadPcb->getRootNode();

    // --- Extract Data using recursion ---
    recursiveExtract(root, *pcbData);

    std::cout << "PcbData populated: " << pcbData->GetLines().size() << " lines/traces, " << pcbData->GetPads().size() << " pads, " << pcbData->GetVias().size() << " vias, " << pcbData->GetZones().size() << " zones." << std::endl;

    return pcbData;
}