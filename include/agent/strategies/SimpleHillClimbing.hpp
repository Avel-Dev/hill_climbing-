#pragma once
#include "agent/strategies/IClimbingStrategy.hpp"

namespace HillExplorer {

class SimpleHillClimbing : public IClimbingStrategy {
public:
    Position2D ChooseNextPosition(const Position2D& current,
                                   float currentHeight,
                                   const Terrain& terrain) override;
    std::string GetName() const override { return "SimpleHillClimbing"; }
    bool ShouldStop(const Position2D& current,
                    float currentHeight,
                    const Terrain& terrain) override;
    void Reset() override;

private:
    Position2D lastPosition{-99999, -99999};
    int stuckCounter = 0;
};

} // namespace HillExplorer