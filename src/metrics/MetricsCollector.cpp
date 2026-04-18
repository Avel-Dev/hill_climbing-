#include "metrics/MetricsCollector.hpp"
#include <raylib.h>
#include <iostream>
#include <fstream>
#include <numeric>

namespace HillExplorer {

void MetricsCollector::RecordStart(const Agent& agent) {
    int id = agent.GetId();
    AgentMetrics m;
    m.agentId = id;
    m.startElevation = agent.GetHeight();
    m.maxElevation = agent.GetHeight();
    m.strategyUsed = agent.GetStrategyName();
    metrics[id] = m;
    startTimes[id] = GetTime();
    startElevations[id] = agent.GetHeight();
}

void MetricsCollector::RecordStep(const Agent& agent) {
    int id = agent.GetId();
    if (metrics.find(id) != metrics.end()) {
        metrics[id].stepsTaken = agent.GetStepsTaken();
        metrics[id].maxElevation = std::max(metrics[id].maxElevation, agent.GetHeight());
    }
}

void MetricsCollector::RecordConvergence(const Agent& agent) {
    int id = agent.GetId();
    if (metrics.find(id) != metrics.end()) {
        auto& m = metrics[id];
        m.converged = true;
        m.convergeTime = GetTime() - startTimes[id];
        m.timeToConverge = m.convergeTime;
        m.stepsTaken = agent.GetStepsTaken();
        m.maxElevation = agent.GetHeight();
        m.elevationGained = m.maxElevation - startElevations[id];
    }
}

AgentMetrics MetricsCollector::GetMetrics(int agentId) const {
    auto it = metrics.find(agentId);
    return (it != metrics.end()) ? it->second : AgentMetrics{};
}

std::vector<AgentMetrics> MetricsCollector::GetAllMetrics() const {
    std::vector<AgentMetrics> result;
    for (const auto& [id, m] : metrics) {
        result.push_back(m);
    }
    return result;
}

void MetricsCollector::PrintSummary() const {
    std::cout << "\n========== SIMULATION RESULTS ==========\n";
    for (const auto& [id, m] : metrics) {
        std::cout << "Agent " << m.agentId << " (" << m.strategyUsed << "):\n";
        std::cout << "  Steps: " << m.stepsTaken << "\n";
        std::cout << "  Max Elevation: " << m.maxElevation << "\n";
        std::cout << "  Elevation Gained: " << m.elevationGained << "\n";
        std::cout << "  Time to Converge: " << m.convergeTime << "s\n";
        std::cout << "  Converged: " << (m.converged ? "Yes" : "No") << "\n\n";
    }
    std::cout << "Average Steps: " << GetAverageSteps() << "\n";
    std::cout << "Average Max Elevation: " << GetAverageElevation() << "\n";
    std::cout << "==========================================\n";
}

void MetricsCollector::ExportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    file << "AgentID,Strategy,Steps,MaxElevation,ElevationGained,TimeToConverge,Converged\n";
    for (const auto& [id, m] : metrics) {
        file << m.agentId << "," << m.strategyUsed << ","
           << m.stepsTaken << "," << m.maxElevation << ","
           << m.elevationGained << "," << m.convergeTime << ","
           << (m.converged ? "1" : "0") << "\n";
    }
}

void MetricsCollector::Clear() {
    metrics.clear();
    startTimes.clear();
    startElevations.clear();
}

float MetricsCollector::GetAverageSteps() const {
    if (metrics.empty()) return 0.0f;
    float total = 0;
    for (const auto& [id, m] : metrics) total += m.stepsTaken;
    return total / metrics.size();
}

float MetricsCollector::GetAverageElevation() const {
    if (metrics.empty()) return 0.0f;
    float total = 0;
    for (const auto& [id, m] : metrics) total += m.maxElevation;
    return total / metrics.size();
}

int MetricsCollector::GetConvergedCount() const {
    int count = 0;
    for (const auto& [id, m] : metrics) {
        if (m.converged) count++;
    }
    return count;
}

} // namespace HillExplorer