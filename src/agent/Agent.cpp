#include "agent/Agent.hpp"
#include "terrain/Terrain.hpp"
#include <raylib.h>

namespace HillExplorer {

Agent::Agent(int id, Position2D startPos, std::shared_ptr<IClimbingStrategy> strategy)
    : id(id), strategy(strategy), color(RED) {
    state.gridPos = startPos;
    state.stepsTaken = 0;
    state.converged = false;
    state.startTime = 0;
    state.convergeTime = 0;
    state.strategyName = strategy ? strategy->GetName() : "Unknown";
    path.push_back(startPos);
}

void Agent::Update(const Terrain& terrain) {
    if (state.converged || !strategy) return;

    float currentHeight = terrain.GetHeight(state.gridPos.x, state.gridPos.z);

    if (strategy->ShouldStop(state.gridPos, currentHeight, terrain)) {
        state.converged = true;
        state.convergeTime = GetTime();
        return;
    }

    Position2D nextPos = strategy->ChooseNextPosition(state.gridPos, currentHeight, terrain);

    if (nextPos != state.gridPos) {
        state.gridPos = nextPos;
        state.stepsTaken++;
        state.height = terrain.GetHeight(nextPos.x, nextPos.z);
        RecordPosition();
    }
}

void Agent::Reset(Position2D startPos) {
    state.gridPos = startPos;
    state.stepsTaken = 0;
    state.converged = false;
    state.startTime = GetTime();
    state.convergeTime = 0;
    path.clear();
    path.push_back(startPos);
    if (strategy) {
        strategy->Reset();
    }
}

void Agent::RecordPosition() {
    if (path.empty() || path.back() != state.gridPos) {
        path.push_back(state.gridPos);
    }
}

} // namespace HillExplorer