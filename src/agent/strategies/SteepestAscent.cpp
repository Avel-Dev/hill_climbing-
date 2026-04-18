#include "agent/strategies/SteepestAscent.hpp"
#include <cmath>

namespace HillExplorer {

SteepestAscent::SteepestAscent(int lookAhead) : lookAhead(lookAhead) {}

Position2D SteepestAscent::ChooseNextPosition(const Position2D& current,
                                               float currentHeight,
                                               const Terrain& terrain) {
    auto neighbors = terrain.GetNeighbors(current.x, current.z, lookAhead);
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

bool SteepestAscent::ShouldStop(const Position2D& current,
                                float currentHeight,
                                const Terrain& terrain) {
    auto neighbors = terrain.GetNeighbors(current.x, current.z, lookAhead);
    for (const auto& neighbor : neighbors) {
        float h = terrain.GetHeight(neighbor.x, neighbor.z);
        if (h > currentHeight + 0.01f) {
            return false;
        }
    }
    return true;
}

void SteepestAscent::Reset() {
    lastPosition = {-99999, -99999};
    stuckCounter = 0;
}

} // namespace HillExplorer