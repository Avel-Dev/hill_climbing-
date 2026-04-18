#pragma once
#include "common/Types.hpp"
#include <memory>

namespace HillExplorer {

class Simulation;
class Renderer;
class MetricsCollector;

class Application {
public:
    Application(int argc, char** argv);
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    std::unique_ptr<Simulation> simulation;
    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<MetricsCollector> metricsCollector;

    int screenWidth = 1600;
    int screenHeight = 900;
    int terrainSize = 100;
    int numAgents = 4;
    float stepInterval = 0.1f;
    bool paused = false;

    bool ParseArguments(int argc, char** argv);
    void SetupSimulation();
    void SetupViewports();
    void HandleInput();
    void ExportResults();
};

} // namespace HillExplorer