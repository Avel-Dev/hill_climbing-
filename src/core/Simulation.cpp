#include "core/Simulation.hpp"
#include "agent/Agent.hpp"
#include "terrain/Terrain.hpp"
#include "metrics/MetricsCollector.hpp"
#include <raylib.h>
#include <algorithm>

namespace HillExplorer {

Simulation::Simulation(std::unique_ptr<Terrain> t) : terrain(std::move(t)) {}

void Simulation::AddAgent(std::shared_ptr<Agent> agent) {
    agents.push_back(agent);
    agent->SetStartTime(GetTime());
}

void Simulation::Step() {
    for (auto& agent : agents) {
        if (!agent->HasConverged()) {
            agent->Update(*terrain);
            if (metricsCollector) {
                UpdateAgentMetrics(*agent);
            }
        }
    }
    currentStep++;
}

void Simulation::Run(int maxSteps) {
    for (int i = 0; i < maxSteps && !IsComplete(); ++i) {
        Step();
        if (stepDelay > 0) {
            WaitTime(stepDelay);
        }
    }
}

void Simulation::Reset() {
    currentStep = 0;
    for (auto& agent : agents) {
        int size = terrain->GetSize();
        Position2D start{rand() % size, rand() % size};
        agent->Reset(start);
    }
}

bool Simulation::IsComplete() const {
    return std::all_of(agents.begin(), agents.end(),
                      [](const auto& a) { return a->HasConverged(); });
}

void Simulation::SetMetricsCollector(std::shared_ptr<MetricsCollector> collector) {
    metricsCollector = collector;
}

void Simulation::UpdateAgentMetrics(const Agent& agent) {
}

} // namespace HillExplorer