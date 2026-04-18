#include "rendering/CameraController.hpp"
#include "agent/Agent.hpp"
#include "terrain/Terrain.hpp"
#include "utils/Math.hpp"
#include <raylib.h>
#include <cmath>

namespace HillExplorer {

CameraController::CameraController()
    : orbitalTarget{0, 0, 0}, orbitalDistance(50.0f),
      orbitalYaw(45.0f), orbitalPitch(30.0f),
      lastMousePos{0, 0}, firstUpdate(true) {}

void CameraController::Update(Camera3D& camera, int viewportWidth, int viewportHeight) {
    if (firstUpdate) {
        lastMousePos = GetMousePosition();
        firstUpdate = false;
    }

    UpdateOrbitalCamera(camera);
}

void CameraController::UpdateOrbitalCamera(Camera3D& camera) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouseDelta = GetMouseDelta();
        orbitalYaw -= mouseDelta.x * 0.3f;
        orbitalPitch -= mouseDelta.y * 0.3f;
        orbitalPitch = Clamp(orbitalPitch, 5.0f, 85.0f);
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        orbitalDistance -= wheel * 5.0f;
        orbitalDistance = Clamp(orbitalDistance, 10.0f, 200.0f);
    }

    float yawRad = orbitalYaw * DEG2RAD;
    float pitchRad = orbitalPitch * DEG2RAD;

    camera.position.x = orbitalTarget.x + orbitalDistance * cosf(pitchRad) * sinf(yawRad);
    camera.position.y = orbitalTarget.y + orbitalDistance * sinf(pitchRad);
    camera.position.z = orbitalTarget.z + orbitalDistance * cosf(pitchRad) * cosf(yawRad);

    camera.target = orbitalTarget;
    camera.up = {0, 1, 0};
}

void CameraController::FollowAgent(Camera3D& camera, const Agent& agent, const Terrain& terrain) {
    Vector3 agentPos = terrain.GridToWorld(agent.GetPosition().x, agent.GetPosition().z);
    orbitalTarget = agentPos;
    UpdateOrbitalCamera(camera);
}

} // namespace HillExplorer