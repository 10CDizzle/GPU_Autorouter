#pragma once

#include "PcbData.h"
#include <vector>
#include <queue>
#include <map>

// Represents a single cell in the routing grid.
struct GridCell {
    // Cost to traverse this cell. Higher for obstacles.
    // Could use std::numeric_limits<float>::infinity() for obstacles.
    float cost = 1.0f;
    // Could also store parent pointers for path reconstruction, etc.
};

// Simple struct for integer coordinates, needed for map keys and general tidiness.
struct GridPoint {
    int x, y;
    bool operator==(const GridPoint& other) const {
        return x == other.x && y == other.y;
    }
    // Needed for using GridPoint as a key in std::map
    bool operator<(const GridPoint& other) const {
        if (x < other.x) return true;
        if (x > other.x) return false;
        return y < other.y;
    }
};

// Represents the 2D routing grid.
class RoutingGrid
{
public:
    RoutingGrid(int width, int height, double resolution);

    // Methods to populate the grid from PcbData
    void AddPadObstacle(const PcbPad& pad, bool isStartOrEnd = false);

    // A* pathfinding
    std::vector<GridPoint> FindPath(GridPoint start, GridPoint end);

    // Coordinate conversion and accessors
    GridPoint WorldToGrid(const wxPoint2DDouble& worldPos) const;
    double GetResolution() const { return m_resolution; }

private:
    // A* helper methods
    double CalculateHeuristic(GridPoint a, GridPoint b);
    std::vector<GridPoint> ReconstructPath(const std::map<GridPoint, GridPoint>& cameFrom, GridPoint current);

    int m_width;
    int m_height;
    double m_resolution; // mm per grid cell
    std::vector<GridCell> m_grid;
};