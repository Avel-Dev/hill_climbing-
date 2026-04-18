#pragma once
#include <string>

namespace HillExplorer {

struct AgentMetrics {
    int agentId;
    int stepsTaken;
    float maxElevation;
    float elevationGained;
    float timeToConverge;
    float convergeTime;
    bool converged;
    std::string strategyUsed;
    float startElevation;

    AgentMetrics() : agentId(0), stepsTaken(0), maxElevation(0),
                     elevationGained(0), timeToConverge(0), convergeTime(0),
                     converged(false), startElevation(0) {}
};

} // namespace HillExplorer