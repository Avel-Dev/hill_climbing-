#include "agent/strategies/StochasticClimbing.hpp"
#include <cmath>

namespace HillExplorer {

StochasticClimbing::StochasticClimbing(float randomChoiceProb, int seed)
    : randomProb(randomChoiceProb), rng(seed) {}

Position2D StochasticClimbing::ChooseNextPosition(const Position2D& current,
                                                   float currentHeight,
                                                   const Terrain& terrain) {
    auto neighbors = terrain.GetNeighbors(current.x, current.z);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::uniform_int_distribution<int> indexDist(0, static_cast<int>(neighbors.size()) - 1);

    if (dist(rng) < randomProb && !neighbors.empty()) {
        lastPosition = current;
        return neighbors[indexDist(rng)];
    }

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

bool StochasticClimbing::ShouldStop(const Position2D& current,
                                    float currentHeight,
                                    const Terrain& terrain) {
    auto neighbors = terrain.GetNeighbors(current.x, current.z);

    if (neighbors.empty()) return true;

    int betterCount = 0;
    for (const auto& neighbor : neighbors) {
        if (terrain.GetHeight(neighbor.x, neighbor.z) > currentHeight + 0.01f) {
            betterCount++;
        }
    }

    stuckCounter++;
    return stuckCounter > 50 || betterCount == 0;
}

void StochasticClimbing::Reset() {
    lastPosition = {-99999, -99999};
    stuckCounter = 0;
}

} // namespace HillExplorer