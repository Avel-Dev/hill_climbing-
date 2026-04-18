#pragma once
#include "agent/strategies/IClimbingStrategy.hpp"
#include <random>

namespace HillExplorer {

class RandomRestart : public IClimbingStrategy {
public:
    RandomRestart(int maxIterationsBeforeRestart = 50, int seed = 0);

    Position2D ChooseNextPosition(const Position2D& current,
                                   float currentHeight,
                                   const Terrain& terrain) override;
    std::string GetName() const override { return "RandomRestart"; }
    bool ShouldStop(const Position2D& current,
                    float currentHeight,
                    const Terrain& terrain) override;
    void Reset() override;

    bool ShouldRestart() const { return iterationsWithoutImprovement >= maxIterations; }
    void MarkRestart() { iterationsWithoutImprovement = 0; lastPosition = {-99999, -99999}; }

private:
    int maxIterations;
    int iterationsWithoutImprovement = 0;
    std::mt19937 rng;
    Position2D lastPosition{-99999, -99999};
    float maxHeightSeen = -999999.0f;
};

} // namespace HillExplorer