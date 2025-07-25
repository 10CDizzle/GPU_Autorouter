#include "RoutingGrid.h"
#include <cmath>
#include <limits>

// A* node structure for the priority queue
struct AStarNode {
    GridPoint pos;
    double f_cost; // g_cost + h_cost

    // For priority_queue (we want the one with the smallest f_cost)
    bool operator>(const AStarNode& other) const {
        return f_cost > other.f_cost;
    }
};

RoutingGrid::RoutingGrid(int width, int height, double resolution)
    : m_width(width), m_height(height), m_resolution(resolution)
{
    m_grid.resize(static_cast<size_t>(width) * height);
}

void RoutingGrid::AddPadObstacle(const PcbPad& pad, bool isStartOrEnd)
{
    // Convert pad dimensions to grid coordinates
    GridPoint center = WorldToGrid(pad.pos);
    int half_width = static_cast<int>(ceil((pad.size.m_x / 2.0) / m_resolution));
    int half_height = static_cast<int>(ceil((pad.size.m_y / 2.0) / m_resolution));

    for (int y = center.y - half_height; y <= center.y + half_height; ++y) {
        for (int x = center.x - half_width; x <= center.x + half_width; ++x) {
            if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
                size_t index = static_cast<size_t>(y) * m_width + x;
                if (isStartOrEnd) {
                    // This is a start/end pad for the current route, so it must be traversable.
                    m_grid[index].cost = 1.0f;
                } else {
                    // This is an obstacle for the current route.
                    m_grid[index].cost = std::numeric_limits<float>::infinity();
                }
            }
        }
    }
}

GridPoint RoutingGrid::WorldToGrid(const wxPoint2DDouble& worldPos) const
{
    return { static_cast<int>(round(worldPos.m_x / m_resolution)),
             static_cast<int>(round(worldPos.m_y / m_resolution)) };
}

std::vector<GridPoint> RoutingGrid::FindPath(GridPoint start, GridPoint end)
{
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;
    std::map<GridPoint, GridPoint> cameFrom;
    std::map<GridPoint, double> gScore;

    gScore[start] = 0;
    openSet.push({start, CalculateHeuristic(start, end)});

    while (!openSet.empty()) {
        GridPoint current = openSet.top().pos;
        openSet.pop();

        if (current == end) {
            return ReconstructPath(cameFrom, current);
        }

        // Check 8 neighbors
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;

                GridPoint neighbor = {current.x + dx, current.y + dy};

                if (neighbor.x < 0 || neighbor.x >= m_width || neighbor.y < 0 || neighbor.y >= m_height) continue;
                
                size_t neighbor_idx = static_cast<size_t>(neighbor.y) * m_width + neighbor.x;
                if (m_grid[neighbor_idx].cost == std::numeric_limits<float>::infinity()) continue;

                double move_cost = (dx != 0 && dy != 0) ? 1.414 : 1.0; // Diagonal vs straight
                double tentative_gScore = gScore.at(current) + move_cost;

                auto it = gScore.find(neighbor);
                if (it == gScore.end() || tentative_gScore < it->second) {
                    cameFrom[neighbor] = current;
                    gScore[neighbor] = tentative_gScore;
                    double fScore = tentative_gScore + CalculateHeuristic(neighbor, end);
                    openSet.push({neighbor, fScore});
                }
            }
        }
    }

    return {}; // No path found
}

double RoutingGrid::CalculateHeuristic(GridPoint a, GridPoint b)
{
    // Diagonal distance (Octile distance)
    int dx = std::abs(a.x - b.x);
    int dy = std::abs(a.y - b.y);
    return (dx + dy) + (1.414 - 2) * std::min(dx, dy);
}

std::vector<GridPoint> RoutingGrid::ReconstructPath(const std::map<GridPoint, GridPoint>& cameFrom, GridPoint current)
{
    std::vector<GridPoint> total_path = {current};
    while (cameFrom.count(current)) {
        current = cameFrom.at(current);
        total_path.insert(total_path.begin(), current);
    }
    return total_path;
}