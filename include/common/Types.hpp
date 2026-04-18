#pragma once
#include <raylib.h>
#include <cstdint>
#include <functional>
#include <string>

namespace HillExplorer {

struct Position2D {
    int x, z;
    bool operator==(const Position2D& other) const {
        return x == other.x && z == other.z;
    }
    bool operator!=(const Position2D& other) const {
        return !(*this == other);
    }
};

struct Position3D {
    float x, y, z;

    operator Vector3() const { return {x, y, z}; }
};

struct AgentState {
    Position2D gridPos;
    float height;
    int stepsTaken;
    bool converged;
    float startTime;
    float convergeTime;
    std::string strategyName;

    AgentState() : gridPos{0, 0}, height(0), stepsTaken(0),
                   converged(false), startTime(0), convergeTime(0) {}
};

using HeightFunc = std::function<float(int x, int z)>;

} // namespace HillExplorer