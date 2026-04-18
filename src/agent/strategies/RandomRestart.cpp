#include "agent/strategies/RandomRestart.hpp"
#include <algorithm>

namespace HillExplorer {

RandomRestart::RandomRestart(int maxIterationsBeforeRestart, int seed)
    : maxIterations(maxIterationsBeforeRestart), rng(seed) {}

Position2D RandomRestart::ChooseNextPosition(const Position2D& current,
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

    if (bestHeight <= currentHeight + 0.01f) {
        iterationsWithoutImprovement++;
    } else {
        iterationsWithoutImprovement = 0;
        maxHeightSeen = std::max(maxHeightSeen, bestHeight);
    }

    lastPosition = current;
    return best;
}

bool RandomRestart::ShouldStop(const Position2D& current,
                              float currentHeight,
                              const Terrain& terrain) {
    return iterationsWithoutImprovement >= maxIterations * 3;
}

void RandomRestart::Reset() {
    iterationsWithoutImprovement = 0;
    lastPosition = {-99999, -99999};
    maxHeightSeen = -999999.0f;
}

} // namespace HillExplorer