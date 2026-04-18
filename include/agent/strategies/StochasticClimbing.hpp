#pragma once
#include "agent/strategies/IClimbingStrategy.hpp"
#include <random>

namespace HillExplorer {

class StochasticClimbing : public IClimbingStrategy {
public:
    explicit StochasticClimbing(float randomChoiceProb = 0.3f, int seed = 0);

    Position2D ChooseNextPosition(const Position2D& current,
                                   float currentHeight,
                                   const Terrain& terrain) override;
    std::string GetName() const override { return "Stochastic_" + std::to_string(static_cast<int>(randomProb * 100)); }
    bool ShouldStop(const Position2D& current,
                    float currentHeight,
                    const Terrain& terrain) override;
    void Reset() override;

private:
    float randomProb;
    std::mt19937 rng;
    Position2D lastPosition{-99999, -99999};
    int stuckCounter = 0;
};

} // namespace HillExplorer