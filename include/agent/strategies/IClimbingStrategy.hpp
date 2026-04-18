#pragma once
#include "common/Types.hpp"
#include "terrain/Terrain.hpp"
#include <vector>
#include <string>
#include <memory>

namespace HillExplorer {

class IClimbingStrategy {
public:
    virtual ~IClimbingStrategy() = default;

    virtual Position2D ChooseNextPosition(
        const Position2D& current,
        float currentHeight,
        const Terrain& terrain
    ) = 0;

    virtual std::string GetName() const = 0;

    virtual bool ShouldStop(const Position2D& current,
                          float currentHeight,
                          const Terrain& terrain) = 0;

    virtual void Reset() {}
};

class StrategyFactory {
public:
    static std::shared_ptr<IClimbingStrategy> CreateSimple();
    static std::shared_ptr<IClimbingStrategy> CreateSteepestAscent(int lookAhead = 1);
    static std::shared_ptr<IClimbingStrategy> CreateStochastic(float probability = 0.2f);
    static std::shared_ptr<IClimbingStrategy> CreateRandomRestart(int maxIterations = 100);
};

} // namespace HillExplorer