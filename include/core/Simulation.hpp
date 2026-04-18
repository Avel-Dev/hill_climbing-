#pragma once
#include "common/Types.hpp"
#include <vector>
#include <memory>

namespace HillExplorer {

class Agent;
class Terrain;
class MetricsCollector;

class Simulation {
public:
    explicit Simulation(std::unique_ptr<Terrain> terrain);

    void AddAgent(std::shared_ptr<Agent> agent);
    void Step();
    void Run(int maxSteps = 1000);
    void Reset();

    const Terrain& GetTerrain() const { return *terrain; }
    const std::vector<std::shared_ptr<Agent>>& GetAgents() const { return agents; }
    int GetCurrentStep() const { return currentStep; }
    bool IsComplete() const;

    void SetStepDelay(float seconds) { stepDelay = seconds; }
    float GetStepDelay() const { return stepDelay; }

    void SetMetricsCollector(std::shared_ptr<MetricsCollector> collector);

private:
    std::unique_ptr<Terrain> terrain;
    std::vector<std::shared_ptr<Agent>> agents;
    int currentStep = 0;
    float stepDelay = 0.0f;
    std::shared_ptr<MetricsCollector> metricsCollector;

    void UpdateAgentMetrics(const Agent& agent);
};

} // namespace HillExplorer