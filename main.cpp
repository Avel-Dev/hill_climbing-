#include "raylib.h"

#include <cmath>
#include <vector>

// ---------------- Terrain ----------------
float GetHeight(float x, float z) {
	float scale = 0.2f;
	return sinf(x * scale) * cosf(z * scale) * 3.0f;
}

void DrawTerrain(int size) {
	for (int x = -size; x < size; x++) {
		for (int z = -size; z < size; z++) {
			float y1 = GetHeight(x, z);
			float y2 = GetHeight(x + 1, z);
			float y3 = GetHeight(x, z + 1);

			DrawLine3D({(float)x, y1, (float)z}, {(float)(x + 1), y2, (float)z},
				 DARKGREEN);

			DrawLine3D({(float)x, y1, (float)z}, {(float)x, y3, (float)(z + 1)},
				 DARKGREEN);
		}
	}
}

// ---------------- Camera Mode ----------------
enum CameraModeCustom { FREE = 0, LOCKED };

struct CameraState {
	Camera3D cam;
	CameraModeCustom mode = FREE;

	// Orbit params
	float distance = 15.0f;
	float yaw = 180.0f;
	float pitch = 20.0f;
};

// ---------------- Main ----------------
int main() {
	InitWindow(1280, 800, "HillExplorer");
	ToggleFullscreen();

	EnableCursor();
	SetTargetFPS(60);

	const int gridSize = 50;
	Vector3 target = {0, 0, 0};

	// --- Cameras ---
	std::vector<CameraState> cameras(4);

	for (int i = 0; i < 4; i++) {
		cameras[i].cam.position = {10.0f + i * 2.0f, 10.0f, 10.0f};
		cameras[i].cam.target = target;
		cameras[i].cam.up = {0, 1, 0};
		cameras[i].cam.fovy = 60.0f;
		cameras[i].cam.projection = CAMERA_PERSPECTIVE;
	}

	int activeCamera = 0;

	while (!WindowShouldClose()) {
		// --- Screen size (IMPORTANT: update every frame) ---
		int w = GetScreenWidth();
		int h = GetScreenHeight();

		Rectangle viewports[4] = {
		  {0, 0, (float)w / 2, (float)h / 2},
		  {(float)w / 2, 0, (float)w / 2, (float)h / 2},
		  {0, (float)h / 2, (float)w / 2, (float)h / 2},
		  {(float)w / 2, (float)h / 2, (float)w / 2, (float)h / 2}};

		// --- Click to select viewport ---
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 m = GetMousePosition();

			for (int i = 0; i < 4; i++) {
				if (CheckCollisionPointRec(m, viewports[i])) {
					activeCamera = i;
				}
			}
		}

		CameraState& camState = cameras[activeCamera];
		Camera3D& cam = camState.cam;

		// --- Switch mode ---
		if (IsKeyPressed(KEY_TAB)) {
			camState.mode = (camState.mode == FREE) ? LOCKED : FREE;

			if (camState.mode == LOCKED) {
				// Reset behind target
				camState.yaw = 180.0f;
				camState.pitch = 20.0f;
				camState.distance = 15.0f;
			}
		}

		// ---------------- UPDATE CAMERA ----------------

		if (camState.mode == FREE) {
			UpdateCamera(&cam, CAMERA_FIRST_PERSON);
		} else if (camState.mode == LOCKED) {
			// Rotate
			if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
				Vector2 delta = GetMouseDelta();

				camState.yaw -= delta.x * 0.3f;
				camState.pitch -= delta.y * 0.3f;

				if (camState.pitch > 89.0f) camState.pitch = 89.0f;
				if (camState.pitch < -89.0f) camState.pitch = -89.0f;
			}

			// Zoom
			camState.distance -= GetMouseWheelMove() * 2.0f;
			if (camState.distance < 3.0f) camState.distance = 3.0f;
			if (camState.distance > 50.0f) camState.distance = 50.0f;

			float yawRad = DEG2RAD * camState.yaw;
			float pitchRad = DEG2RAD * camState.pitch;

			cam.position.x =
			  target.x + camState.distance * cosf(pitchRad) * sinf(yawRad);
			cam.position.y = target.y + camState.distance * sinf(pitchRad);
			cam.position.z =
			  target.z + camState.distance * cosf(pitchRad) * cosf(yawRad);

			cam.target = target;
		}

		// ---------------- DRAW ----------------
		BeginDrawing();
		ClearBackground(BLACK);

		for (int i = 0; i < 4; i++) {
			Rectangle vp = viewports[i];

			BeginScissorMode((int)vp.x, (int)vp.y, (int)vp.width, (int)vp.height);

			BeginMode3D(cameras[i].cam);

			DrawTerrain(gridSize);
			DrawSphere(target, 0.4f, RED);

			EndMode3D();
			EndScissorMode();

			DrawRectangleLinesEx(vp, 2, (i == activeCamera) ? YELLOW : DARKGRAY);
		}

		// UI
		DrawText("Click viewport to select | TAB: Switch Mode", 20, 20, 20, WHITE);

		const char* modeText =
		  (cameras[activeCamera].mode == FREE) ? "FREE MODE" : "LOCKED MODE";
		DrawText(TextFormat("Active Camera: %d (%s)", activeCamera, modeText), 20, 50,
		         20, YELLOW);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
