#pragma once
#include "agent/strategies/IClimbingStrategy.hpp"

namespace HillExplorer {

class SteepestAscent : public IClimbingStrategy {
public:
    explicit SteepestAscent(int lookAhead = 1);

    Position2D ChooseNextPosition(const Position2D& current,
                                   float currentHeight,
                                   const Terrain& terrain) override;
    std::string GetName() const override { return "SteepestAscent_" + std::to_string(lookAhead); }
    bool ShouldStop(const Position2D& current,
                    float currentHeight,
                    const Terrain& terrain) override;
    void Reset() override;

private:
    int lookAhead;
    Position2D lastPosition{-99999, -99999};
    int stuckCounter = 0;
};

} // namespace HillExplorer