#include "raylib.h"
#include "raymath.h"

#include <cmath>
#include <vector>

// ---------------- Utility ----------------
template<typename T> T ClampValue(T v, T min, T max) {
	if (v < min) return min;
	if (v > max) return max;
	return v;
}

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

// ---------------- Camera ----------------
enum CameraModeCustom { FREE = 0, LOCKED };

struct CameraState {
	Camera3D cam;
	CameraModeCustom mode = LOCKED;

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

	std::vector<Vector3> targets = {{-10, 0, -10}, {10, 0, -10}, {-10, 0, 10}, {10, 0, 10}};

	std::vector<CameraState> cameras(4);

	// Init cameras
	for (int i = 0; i < 4; i++) {
		cameras[i].cam.target = targets[i];
		cameras[i].cam.up = {0, 1, 0};
		cameras[i].cam.fovy = 60.0f;
		cameras[i].cam.projection = CAMERA_PERSPECTIVE;
	}

	int activeCamera = 0;

	// Render textures
	RenderTexture2D renderTex[4];

	int screenW = GetScreenWidth();
	int screenH = GetScreenHeight();

	for (int i = 0; i < 4; i++) {
		renderTex[i] = LoadRenderTexture(screenW / 2, screenH / 2);
	}

	while (!WindowShouldClose()) {
		// --- Handle resize ---
		int w = GetScreenWidth();
		int h = GetScreenHeight();

		static int prevW = w, prevH = h;
		if (w != prevW || h != prevH) {
			for (int i = 0; i < 4; i++) {
				UnloadRenderTexture(renderTex[i]);
				renderTex[i] = LoadRenderTexture(w / 2, h / 2);
			}
			prevW = w;
			prevH = h;
		}

		Rectangle viewports[4] = {
		  {0, 0, (float)w / 2, (float)h / 2},
		  {(float)w / 2, 0, (float)w / 2, (float)h / 2},
		  {0, (float)h / 2, (float)w / 2, (float)h / 2},
		  {(float)w / 2, (float)h / 2, (float)w / 2, (float)h / 2}};

		// --- Mouse select viewport ---
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

		// --- Toggle mode ---
		if (IsKeyPressed(KEY_TAB)) {
			camState.mode = (camState.mode == FREE) ? LOCKED : FREE;
		}

		// --- Update active camera ---
		if (camState.mode == FREE) {
			UpdateCamera(&cam, CAMERA_FIRST_PERSON);
		} else {
			if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
				Vector2 delta = GetMouseDelta();
				camState.yaw -= delta.x * 0.3f;
				camState.pitch -= delta.y * 0.3f;
			}

			camState.pitch = ClampValue(camState.pitch, -89.0f, 89.0f);

			camState.distance -= GetMouseWheelMove() * 2.0f;
			camState.distance = ClampValue(camState.distance, 3.0f, 50.0f);

			Vector3 target = targets[activeCamera];

			float yawRad = DEG2RAD * camState.yaw;
			float pitchRad = DEG2RAD * camState.pitch;

			cam.position.x =
			  target.x + camState.distance * cosf(pitchRad) * sinf(yawRad);
			cam.position.y = target.y + camState.distance * sinf(pitchRad);
			cam.position.z =
			  target.z + camState.distance * cosf(pitchRad) * cosf(yawRad);

			cam.target = target;
		}

		// -------- Render each camera to texture --------
		for (int i = 0; i < 4; i++) {
			BeginTextureMode(renderTex[i]);
			ClearBackground(BLACK);

			BeginMode3D(cameras[i].cam);

			DrawTerrain(gridSize);
			for (int j = 0; j < 4; j++)
				DrawSphere(targets[j], 0.5f, RED);

			EndMode3D();
			EndTextureMode();
		}

		// -------- Draw to screen --------
		BeginDrawing();
		ClearBackground(BLACK);

		for (int i = 0; i < 4; i++) {
			Rectangle vp = viewports[i];

			DrawTexturePro(renderTex[i].texture,
				     {0, 0, (float)renderTex[i].texture.width,
				      -(float)renderTex[i].texture.height},
				     vp, {0, 0}, 0.0f, WHITE);

			DrawRectangleLinesEx(vp, 2, (i == activeCamera) ? YELLOW : DARKGRAY);
		}

		DrawText("Click viewport | TAB switch mode | RMB rotate | Scroll zoom", 20, 20,
		         20, WHITE);

		EndDrawing();
	}

	for (int i = 0; i < 4; i++) {
		UnloadRenderTexture(renderTex[i]);
	}

	CloseWindow();
	return 0;
}
