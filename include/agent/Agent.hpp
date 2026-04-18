#pragma once
#include "common/Types.hpp"
#include "agent/strategies/IClimbingStrategy.hpp"
#include <memory>
#include <vector>

namespace HillExplorer {

class Terrain;

class Agent {
public:
    Agent(int id, Position2D startPos, std::shared_ptr<IClimbingStrategy> strategy);

    void Update(const Terrain& terrain);

    int GetId() const { return id; }
    const Position2D& GetPosition() const { return state.gridPos; }
    float GetHeight() const { return state.height; }
    int GetStepsTaken() const { return state.stepsTaken; }
    bool HasConverged() const { return state.converged; }
    const AgentState& GetState() const { return state; }
    const std::vector<Position2D>& GetPath() const { return path; }
    std::string GetStrategyName() const { return state.strategyName; }

    void Reset(Position2D startPos);
    void SetColor(Color c) { color = c; }
    Color GetColor() const { return color; }

    void SetStartTime(float time) { state.startTime = time; }
    float GetStartTime() const { return state.startTime; }

private:
    int id;
    AgentState state;
    std::shared_ptr<IClimbingStrategy> strategy;
    std::vector<Position2D> path;
    Color color;

    void RecordPosition();
};

} // namespace HillExplorer