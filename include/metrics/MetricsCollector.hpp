#pragma once
#include "AgentMetrics.hpp"
#include "agent/Agent.hpp"
#include <vector>
#include <map>
#include <string>

namespace HillExplorer {

class MetricsCollector {
public:
    void RecordStart(const Agent& agent);
    void RecordStep(const Agent& agent);
    void RecordConvergence(const Agent& agent);

    AgentMetrics GetMetrics(int agentId) const;
    std::vector<AgentMetrics> GetAllMetrics() const;
    void PrintSummary() const;
    void ExportToCSV(const std::string& filename) const;
    void Clear();

    float GetAverageSteps() const;
    float GetAverageElevation() const;
    int GetConvergedCount() const;

private:
    std::map<int, AgentMetrics> metrics;
    std::map<int, float> startTimes;
    std::map<int, float> startElevations;
};

} // namespace HillExplorer