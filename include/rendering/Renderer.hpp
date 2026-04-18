#pragma once
#include "common/Types.hpp"
#include <vector>
#include <memory>
#include <string>

namespace HillExplorer {

class Terrain;
class Agent;
class Simulation;

struct Viewport {
    Rectangle rect;
    Camera3D camera;
    int trackedAgentId;
    std::string label;

    Viewport() : rect{0, 0, 0, 0}, trackedAgentId(-1) {}
};

class Renderer {
public:
    Renderer(int screenWidth, int screenHeight);
    ~Renderer();

    void Initialize(const Terrain& terrain);
    void Render(const Simulation& simulation);
    void RenderViewport(const Viewport& vp, const Simulation& simulation);

    void AddViewport(const Rectangle& rect, const Camera3D& camera, int trackedAgent = -1, const std::string& label = "");
    void ClearViewports();

    void UpdateCamera(int viewportIndex);

    void DrawUI(const Simulation& simulation);
    void DrawMetrics(const Simulation& simulation, int x, int y);

    void ToggleDebugMode() { debugMode = !debugMode; }
    bool IsDebugMode() const { return debugMode; }

    bool ShouldClose() const;
    void Shutdown();

    int GetScreenWidth() const { return screenWidth; }
    int GetScreenHeight() const { return screenHeight; }

private:
    int screenWidth, screenHeight;
    std::vector<Viewport> viewports;
    bool debugMode = false;
    bool initialized = false;

    Mesh terrainMesh{};
    Model terrainModel{};
    Texture2D heightmapTexture{};
    Image heightmapImage{};

    void GenerateTerrainMesh(const Terrain& terrain);
    void DrawAgents(const Simulation& simulation);
    void DrawAgentPath(const Agent& agent, const Terrain& terrain);
    void DrawTerrainWireframe(const Terrain& terrain);
    void DrawConvergenceMarkers(const Simulation& simulation);

    Color GetHeightColor(float height, float minHeight, float maxHeight);
};

} // namespace HillExplorer