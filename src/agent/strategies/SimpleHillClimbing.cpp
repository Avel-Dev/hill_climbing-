#include "agent/strategies/SimpleHillClimbing.hpp"

namespace HillExplorer {

Position2D SimpleHillClimbing::ChooseNextPosition(const Position2D& current,
                                                   float currentHeight,
                                                   const Terrain& terrain) {
    auto neighbors = terrain.GetNeighbors(current.x, current.z);
    Position2D best = current;
    float bestHeight = currentHeight;

    for (const auto& neighbor : neighbors) {
        float h = terrain.GetHeight(neighbor.x, neighbor.z);
        if (h > bestHeight) {
            bestHeight = h;
            best = neighbor;
        }
    }

    lastPosition = current;
    return best;
}

bool SimpleHillClimbing::ShouldStop(const Position2D& current,
                                    float currentHeight,
                                    const Terrain& terrain) {
    auto neighbors = terrain.GetNeighbors(current.x, current.z);
    for (const auto& neighbor : neighbors) {
        if (terrain.GetHeight(neighbor.x, neighbor.z) > currentHeight + 0.01f) {
            return false;
        }
    }
    return true;
}

void SimpleHillClimbing::Reset() {
    lastPosition = {-99999, -99999};
    stuckCounter = 0;
}

} // namespace HillExplorer