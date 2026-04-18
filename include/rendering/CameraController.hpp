#pragma once
#include "common/Types.hpp"
#include <memory>

namespace HillExplorer {

class Agent;
class Terrain;

class CameraController {
public:
    CameraController();

    void Update(Camera3D& camera, int viewportWidth, int viewportHeight);
    void FollowAgent(Camera3D& camera, const Agent& agent, const Terrain& terrain);

    void SetOrbitalTarget(const Vector3& target) { orbitalTarget = target; }
    void SetOrbitalDistance(float dist) { orbitalDistance = dist; }
    void SetOrbitalAngle(float yaw, float pitch) { orbitalYaw = yaw; orbitalPitch = pitch; }

private:
    Vector3 orbitalTarget;
    float orbitalDistance;
    float orbitalYaw;
    float orbitalPitch;
    Vector2 lastMousePos;
    bool firstUpdate;

    void UpdateOrbitalCamera(Camera3D& camera);
};

} // namespace HillExplorer