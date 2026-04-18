#include "agent/strategies/IClimbingStrategy.hpp"
#include "agent/strategies/SimpleHillClimbing.hpp"
#include "agent/strategies/SteepestAscent.hpp"
#include "agent/strategies/StochasticClimbing.hpp"
#include "agent/strategies/RandomRestart.hpp"

namespace HillExplorer {

std::shared_ptr<IClimbingStrategy> StrategyFactory::CreateSimple() {
    return std::make_shared<SimpleHillClimbing>();
}

std::shared_ptr<IClimbingStrategy> StrategyFactory::CreateSteepestAscent(int lookAhead) {
    return std::make_shared<SteepestAscent>(lookAhead);
}

std::shared_ptr<IClimbingStrategy> StrategyFactory::CreateStochastic(float probability) {
    return std::make_shared<StochasticClimbing>(probability);
}

std::shared_ptr<IClimbingStrategy> StrategyFactory::CreateRandomRestart(int maxIterations) {
    return std::make_shared<RandomRestart>(maxIterations);
}

} // namespace HillExplorer