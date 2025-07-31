#ifndef PCB_DATA_H
#define PCB_DATA_H

#include <vector>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/geometry.h>


struct PcbLine {
    wxPoint2DDouble start;
    wxPoint2DDouble end;
    double width;
    wxString layer;
    int netId = -1;
};

struct PcbVia {
    wxPoint2DDouble pos;
    double size;
    double drill;
    wxString fromLayer;
    wxString toLayer;
    int netId = -1;
};

struct PcbZone {
    int netId = -1;
    wxString layer;
    std::vector<wxPoint2DDouble> polygon;
};

struct PcbPad {
    wxPoint2DDouble pos;
    wxPoint2DDouble size;
    wxString shape; // e.g., "rect", "circle", "oval"
    double rotation = 0.0;
    wxString layer;
    int netId = -1;
};

class PcbData {
public:
    PcbData() = default;

    void Clear();
    void AddLine(const PcbLine& line);
    void AddPad(const PcbPad& pad);
    void AddVia(const PcbVia& via);
    void AddZone(const PcbZone& zone);
    void AddNet(const wxString& netName);

    // Accessors
    const std::vector<PcbLine>& GetLines() const { return m_lines; }
    const std::vector<PcbPad>& GetPads() const { return m_pads; }
    const std::vector<PcbVia>& GetVias() const { return m_vias; }
    const std::vector<PcbZone>& GetZones() const { return m_zones; }
    const std::vector<wxString>& GetNets() const { return m_nets; }
    std::vector<wxString> GetUniqueLayerNames() const;
    wxRect2DDouble GetBoundingBox() const;

    int GetNetIdByName(const wxString& netName) const;

private:
    std::vector<PcbLine> m_lines;
    std::vector<PcbPad> m_pads;
    std::vector<PcbVia> m_vias;
    std::vector<PcbZone> m_zones;
    std::vector<wxString> m_nets;
    wxRect2DDouble m_boundingBox;
};

#endif // PCB_DATA_H