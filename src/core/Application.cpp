#include "core/Application.hpp"
#include "core/Simulation.hpp"
#include "rendering/Renderer.hpp"
#include "terrain/Terrain.hpp"
#include "terrain/HeightFunction.hpp"
#include "agent/Agent.hpp"
#include "agent/strategies/IClimbingStrategy.hpp"
#include "metrics/MetricsCollector.hpp"
#include <raylib.h>
#include <ctime>
#include <iostream>
#include <cstring>
#include <string>

namespace HillExplorer {

Application::Application(int argc, char** argv) {
    ParseArguments(argc, argv);
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

Application::~Application() {
    Shutdown();
}

bool Application::ParseArguments(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
            screenWidth = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
            screenHeight = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            terrainSize = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            numAgents = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0) {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  -w WIDTH     Screen width (default: 1600)\n"
                      << "  -h HEIGHT    Screen height (default: 900)\n"
                      << "  -s SIZE      Terrain size (default: 100)\n"
                      << "  -a AGENTS    Number of agents (default: 4)\n"
                      << "  --help       Show this help message\n";
            return false;
        }
    }
    return true;
}

bool Application::Initialize() {
    renderer = std::make_unique<Renderer>(screenWidth, screenHeight);
    metricsCollector = std::make_shared<MetricsCollector>();

    SetupSimulation();
    SetupViewports();

    renderer->Initialize(simulation->GetTerrain());

    return true;
}

void Application::SetupSimulation() {
    auto heightFunc = std::make_unique<PerlinHeight>(42, 4, 0.5f);
    auto terrain = std::make_unique<Terrain>(terrainSize, 1.0f, std::move(heightFunc));

    simulation = std::make_unique<Simulation>(std::move(terrain));
    simulation->SetMetricsCollector(metricsCollector);

    Color colors[] = {RED, BLUE, GREEN, PURPLE, ORANGE, YELLOW, PINK, SKYBLUE};

    for (int i = 0; i < numAgents; ++i) {
        Position2D start{rand() % terrainSize, rand() % terrainSize};

        std::shared_ptr<IClimbingStrategy> strategy;
        switch (i % 4) {
            case 0: strategy = StrategyFactory::CreateSimple(); break;
            case 1: strategy = StrategyFactory::CreateSteepestAscent(2); break;
            case 2: strategy = StrategyFactory::CreateStochastic(0.3f); break;
            case 3: strategy = StrategyFactory::CreateRandomRestart(50); break;
        }

        auto agent = std::make_shared<Agent>(i, start, strategy);
        agent->SetColor(colors[i % 8]);
        simulation->AddAgent(agent);
        metricsCollector->RecordStart(*agent);
    }
}

void Application::SetupViewports() {
    int halfW = screenWidth / 2;
    int halfH = screenHeight / 2;

    Camera3D overviewCam = {
        {terrainSize * 0.7f, terrainSize * 0.8f, terrainSize * 0.7f},
        {terrainSize * 0.5f, 0.0f, terrainSize * 0.5f},
        {0.0f, 1.0f, 0.0f},
        45.0f,
        CAMERA_PERSPECTIVE
    };

    renderer->AddViewport({0, 0, (float)halfW, (float)halfH}, overviewCam, -1, "Overview");

    for (int i = 0; i < numAgents && i < 3; ++i) {
        Camera3D agentCam = {
            {terrainSize * 0.5f, 40.0f, terrainSize * 0.5f},
            {terrainSize * 0.5f, 0.0f, terrainSize * 0.5f},
            {0.0f, 1.0f, 0.0f},
            60.0f,
            CAMERA_PERSPECTIVE
        };

        float x = (i == 0) ? (float)halfW : 0;
        float y = (i == 0) ? 0 : (float)halfH;
        renderer->AddViewport({x, y, (float)halfW, (float)halfH}, agentCam, i,
                               TextFormat("Agent %d", i));
    }
}

void Application::Run() {
    float stepTimer = 0.0f;

    while (!renderer->ShouldClose()) {
        float dt = GetFrameTime();

        HandleInput();

        if (!paused) {
            stepTimer += dt;
            if (stepTimer >= stepInterval) {
                simulation->Step();
                stepTimer = 0.0f;

                for (const auto& agent : simulation->GetAgents()) {
                    if (agent->HasConverged()) {
                        metricsCollector->RecordConvergence(*agent);
                    } else {
                        metricsCollector->RecordStep(*agent);
                    }
                }
            }
        }

        for (size_t i = 0; i < 4; ++i) {
            renderer->UpdateCamera(static_cast<int>(i));
        }

        renderer->Render(*simulation);
    }

    ExportResults();
}

void Application::HandleInput() {
    if (IsKeyPressed(KEY_D)) {
        renderer->ToggleDebugMode();
    }
    if (IsKeyPressed(KEY_R)) {
        simulation->Reset();
        metricsCollector->Clear();
        for (const auto& agent : simulation->GetAgents()) {
            metricsCollector->RecordStart(*agent);
        }
    }
    if (IsKeyPressed(KEY_SPACE)) {
        paused = !paused;
    }
}

void Application::ExportResults() {
    metricsCollector->ExportToCSV("hill_explorer_results.csv");
    metricsCollector->PrintSummary();
    std::cout << "Results exported to hill_explorer_results.csv\n";
}

void Application::Shutdown() {
    if (renderer) {
        renderer->Shutdown();
    }
}

} // namespace HillExplorer