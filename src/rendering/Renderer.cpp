#include "rendering/Renderer.hpp"
#include "rendering/CameraController.hpp"
#include "agent/Agent.hpp"
#include "terrain/Terrain.hpp"
#include "core/Simulation.hpp"
#include <raylib.h>
#include <cmath>
#include <sstream>

namespace HillExplorer {

Renderer::Renderer(int width, int height)
    : screenWidth(width), screenHeight(height), initialized(false) {
    InitWindow(width, height, "HillExplorer - Multi-Agent Hill Climbing Simulation");
    SetTargetFPS(60);
}

Renderer::~Renderer() {
    Shutdown();
}

void Renderer::Initialize(const Terrain& terrain) {
    if (initialized) return;

    GenerateTerrainMesh(terrain);
    initialized = true;
}

void Renderer::GenerateTerrainMesh(const Terrain& terrain) {
    int size = terrain.GetSize();

    if (heightmapImage.data) UnloadImage(heightmapImage);
    heightmapImage = GenImageColor(size, size, WHITE);

    float minH = terrain.GetMinHeight();
    float maxH = terrain.GetMaxHeight();

    for (int z = 0; z < size; ++z) {
        for (int x = 0; x < size; ++x) {
            float h = terrain.GetHeight(x, z);
            float normalized = (h - minH) / (maxH - minH + 0.001f);
            Color c = {
                (unsigned char)(normalized * 255),
                (unsigned char)(normalized * 255),
                (unsigned char)(normalized * 255),
                255
            };
            ImageDrawPixel(&heightmapImage, x, z, c);
        }
    }

    if (heightmapTexture.id) UnloadTexture(heightmapTexture);
    heightmapTexture = LoadTextureFromImage(heightmapImage);

    terrainMesh = GenMeshHeightmap(heightmapImage, {size, maxH - minH, size});
    terrainModel = LoadModelFromMesh(terrainMesh);

    Material mat = terrainModel.materials[0];
    mat.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
}

void Renderer::Render(const Simulation& simulation) {
    if (!initialized) return;

    BeginDrawing();
    ClearBackground(DARKGRAY);

    for (size_t i = 0; i < viewports.size(); ++i) {
        RenderViewport(viewports[i], simulation);
    }

    DrawUI(simulation);

    EndDrawing();
}

void Renderer::RenderViewport(const Viewport& vp, const Simulation& simulation) {
    BeginScissorMode(static_cast<int>(vp.rect.x), static_cast<int>(vp.rect.y),
                     static_cast<int>(vp.rect.width), static_cast<int>(vp.rect.height));

    ClearBackground({20, 20, 30, 255});

    Camera3D cam = vp.camera;

    if (vp.trackedAgentId >= 0) {
        for (const auto& agent : simulation.GetAgents()) {
            if (agent->GetId() == vp.trackedAgentId) {
                Vector3 pos = simulation.GetTerrain().GridToWorld(
                    agent->GetPosition().x, agent->GetPosition().z);
                cam.target = pos;
                cam.position = {pos.x + 20, pos.y + 30, pos.z + 20};
                break;
            }
        }
    }

    BeginMode3D(cam);

    DrawModel(terrainModel, {0, 0, 0}, 1.0f, WHITE);

    if (debugMode) {
        DrawTerrainWireframe(simulation.GetTerrain());
    }

    DrawAgents(simulation);
    DrawConvergenceMarkers(simulation);

    EndMode3D();
    EndScissorMode();

    DrawRectangleLines(static_cast<int>(vp.rect.x), static_cast<int>(vp.rect.y),
                       static_cast<int>(vp.rect.width), static_cast<int>(vp.rect.height),
                       WHITE);

    if (!vp.label.empty()) {
        DrawText(vp.label.c_str(), static_cast<int>(vp.rect.x) + 5,
                 static_cast<int>(vp.rect.y) + 5, 16, WHITE);
    }
}

void Renderer::DrawAgents(const Simulation& simulation) {
    const auto& terrain = simulation.GetTerrain();

    for (const auto& agent : simulation.GetAgents()) {
        Vector3 pos = terrain.GridToWorld(agent->GetPosition().x, agent->GetPosition().z);

        Color c = agent->GetColor();
        if (agent->HasConverged()) {
            c.r = c.r / 2;
            c.g = c.g / 2;
            c.b = c.b / 2;
        }

        DrawSphere(pos, 0.5f, c);

        if (agent->GetPath().size() > 1) {
            DrawAgentPath(*agent, terrain);
        }
    }
}

void Renderer::DrawAgentPath(const Agent& agent, const Terrain& terrain) {
    const auto& path = agent.GetPath();
    Color c = agent.GetColor();
    c.a = 128;

    for (size_t i = 1; i < path.size(); ++i) {
        Vector3 p1 = terrain.GridToWorld(path[i-1].x, path[i-1].z);
        Vector3 p2 = terrain.GridToWorld(path[i].x, path[i].z);
        p1.y += 0.3f;
        p2.y += 0.3f;
        DrawLine3D(p1, p2, c);
    }
}

void Renderer::DrawConvergenceMarkers(const Simulation& simulation) {
    const auto& terrain = simulation.GetTerrain();

    for (const auto& agent : simulation.GetAgents()) {
        if (agent->HasConverged()) {
            Vector3 pos = terrain.GridToWorld(agent->GetPosition().x, agent->GetPosition().z);
            pos.y += 2.0f;
            DrawCylinder({pos.x, pos.y - 1.0f, pos.z}, 0.1f, 0.1f, 2.0f, 8, GREEN);
        }
    }
}

void Renderer::DrawTerrainWireframe(const Terrain& terrain) {
    int size = terrain.GetSize();
    for (int z = 0; z < size; z += 5) {
        for (int x = 0; x < size; x += 5) {
            Vector3 p = terrain.GridToWorld(x, z);
            DrawCubeWires(p, 1, 1, 1, {100, 100, 100, 50});
        }
    }
}

void Renderer::DrawUI(const Simulation& simulation) {
    const auto& agents = simulation.GetAgents();
    int y = 10;
    int x = 10;

    DrawText("HillExplorer", x, y, 24, WHITE);
    y += 30;
    DrawText(TextFormat("Step: %d", simulation.GetCurrentStep()), x, y, 16, LIGHTGRAY);
    y += 20;

    int converged = 0;
    for (const auto& a : agents) {
        if (a->HasConverged()) converged++;
    }
    DrawText(TextFormat("Agents: %d/%d converged", converged, (int)agents.size()), x, y, 16, LIGHTGRAY);
    y += 25;

    DrawText("Controls: Click+Drag=Rotate, Scroll=Zoom, D=Debug, R=Reset, Space=Pause", x, screenHeight - 25, 14, GRAY);

    DrawMetrics(simulation, screenWidth - 250, 10);
}

void Renderer::DrawMetrics(const Simulation& simulation, int x, int y) {
    const auto& agents = simulation.GetAgents();
    int lineHeight = 16;

    DrawRectangle(x - 5, y, 240, agents.size() * lineHeight + 50, {0, 0, 0, 180});

    DrawText("Agent Metrics", x, y, 18, WHITE);
    y += 25;

    for (const auto& agent : agents) {
        std::string status = agent->HasConverged() ? "[DONE]" : "[climbing]";
        const char* text = TextFormat("Agent %d: steps=%d %s",
            agent->GetId(), agent->GetStepsTaken(), status.c_str());
        DrawText(text, x, y, 14, agent->GetColor());
        y += lineHeight;
    }
}

void Renderer::AddViewport(const Rectangle& rect, const Camera3D& camera, int trackedAgent, const std::string& label) {
    Viewport vp;
    vp.rect = rect;
    vp.camera = camera;
    vp.trackedAgentId = trackedAgent;
    vp.label = label;
    viewports.push_back(vp);
}

void Renderer::ClearViewports() {
    viewports.clear();
}

void Renderer::UpdateCamera(int viewportIndex) {
    if (viewportIndex < 0 || viewportIndex >= static_cast<int>(viewports.size())) return;

    Viewport& vp = viewports[viewportIndex];

    if (vp.trackedAgentId < 0) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, vp.rect)) {
                Vector2 delta = GetMouseDelta();
                Camera3D cam = vp.camera;

                float yaw = atan2f(cam.position.x - cam.target.x, cam.position.z - cam.target.z);
                yaw -= delta.x * 0.01f;

                float dx = cam.position.x - cam.target.x;
                float dz = cam.position.z - cam.target.z;
                float dist = sqrtf(dx*dx + dz*dz);

                cam.position.x = cam.target.x + dist * sinf(yaw);
                cam.position.z = cam.target.z + dist * cosf(yaw);

                vp.camera = cam;
            }
        }
    }
}

bool Renderer::ShouldClose() const {
    return WindowShouldClose();
}

void Renderer::Shutdown() {
    if (!initialized) return;

    if (terrainModel.meshCount > 0) {
        UnloadModel(terrainModel);
    }
    if (heightmapTexture.id) {
        UnloadTexture(heightmapTexture);
    }
    if (heightmapImage.data) {
        UnloadImage(heightmapImage);
    }

    CloseWindow();
    initialized = false;
}

Color Renderer::GetHeightColor(float height, float minHeight, float maxHeight) {
    float t = (height - minHeight) / (maxHeight - minHeight + 0.001f);
    if (t < 0.3f) return DARKGREEN;
    if (t < 0.6f) return GREEN;
    if (t < 0.8f) return ORANGE;
    return WHITE;
}

} // namespace HillExplorer