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

    void extractNets(const SexpNode& root, PcbData& pcbData) {
        if (!root.isList()) return;
        for (const auto& node : root.getList()) {
            if (node.isList() && !node.getList().empty() && node.getList()[0].isAtom() && node.getList()[0].getAtom() == "net") {
                if (node.getList().size() > 2 && node.getList()[2].isAtom()) {
                    pcbData.AddNet(node.getList()[2].getAtom());
                }
            }
        }
    }

    void extractBoardOutline(const SexpNode& root, PcbData& pcbData) {
        if (!root.isList()) return;
        for (const auto& node : root.getList()) {
            if (node.isList() && !node.getList().empty() && node.getList()[0].isAtom() && node.getList()[0].getAtom() == "gr_line") {
                const SexpNode* layerNode = findNode(node, "layer");
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
        }
    }

    void extractPads(const SexpNode& root, PcbData& pcbData) {
        if (!root.isList()) return;
        for (const auto& node : root.getList()) {
            if (node.isList() && !node.getList().empty() && node.getList()[0].isAtom() && node.getList()[0].getAtom() == "pad") {
                PcbPad pad;
                const SexpNode* atNode = findNode(node, "at");
                const SexpNode* sizeNode = findNode(node, "size");
                const SexpNode* layersNode = findNode(node, "layers");
                const SexpNode* netNode = findNode(node, "net");

                if (parsePoint(atNode, pad.pos) && parseSize(sizeNode, pad.size) &&
                    node.getList().size() > 2 && node.getList()[2].isAtom() &&
                    layersNode && layersNode->getList().size() > 1 &&
                    netNode && netNode->getList().size() > 1) {
                    pad.shape = node.getList()[2].getAtom();
                    pad.layer = layersNode->getList()[1].getAtom();
                    pad.netId = std::stoi(netNode->getList()[1].getAtom());
                    pcbData.AddPad(pad);
                }
            }
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

    // --- Extract Data ---
    extractNets(root, *pcbData);
    extractBoardOutline(root, *pcbData);
    extractPads(root, *pcbData);

    std::cout << "PcbData object populated with " << pcbData->GetLines().size() << " outline segments and " << pcbData->GetPads().size() << " pads." << std::endl;

    return pcbData;
}