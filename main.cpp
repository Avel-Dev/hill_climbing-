#include "raylib.h"

#include <cmath>
#include <vector>

// Simple terrain
float GetHeight(float x, float z) {
	float scale = 0.2f;
	return sinf(x * scale) * cosf(z * scale) * 3.0f;
}

// Draw terrain grid
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

int main() {
	InitWindow(1280, 800, "HillExplorer");

	// Make it borderless fullscreen
	ToggleFullscreen();
	DisableCursor();
	SetTargetFPS(60);

	// --- Create 4 cameras ---
	std::vector<Camera3D> cameras(4);
	float w = GetScreenWidth();
	float h = GetScreenHeight();

	Rectangle viewports[4] = {
	  {0, 0, (float)(w / 2), (float)(h / 2)},
	  {(float)(w / 2), 0, (float)(w / 2), (float)(h / 2)},
	  {0, (float)(h / 2), (float)(w / 2), (float)(h / 2)},
	  {(float)(w / 2), (float)(h / 2), (float)(w / 2), (float)(h / 2)}};

	for (int i = 0; i < 4; i++) {
		cameras[i].position = {10.0f + i * 2.0f, 10.0f, 10.0f};
		cameras[i].target = {0.0f, 0.0f, 0.0f};
		cameras[i].up = {0.0f, 1.0f, 0.0f};
		cameras[i].fovy = 60.0f;
		cameras[i].projection = CAMERA_PERSPECTIVE;
	}

	int activeCamera = 0;
	bool cameraControl = false;
	const int gridSize = 50;

	while (!WindowShouldClose()) {
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 m = GetMousePosition();

			if (m.x < w / 2 && m.y < h / 2)
				activeCamera = 0;
			else if (m.x >= w / 2 && m.y < h / 2)
				activeCamera = 1;
			else if (m.x < w / 2 && m.y >= h / 2)
				activeCamera = 2;
			else
				activeCamera = 3;
			DisableCursor();
			cameraControl = true;
		}

		if (IsKeyPressed(KEY_Q)) {
			EnableCursor();
			cameraControl = false;
		}
		// --- Update only active camera ---
		UpdateCamera(&cameras[activeCamera], CAMERA_FIRST_PERSON);

		// --- Draw ---
		BeginDrawing();
		ClearBackground(BLACK);

		for (int i = 0; i < 4; i++) {
			Rectangle vp = viewports[i];

			// Enable clipping to viewport
			BeginScissorMode((int)vp.x, (int)vp.y, (int)vp.width, (int)vp.height);

			// Set viewport
			BeginMode3D(cameras[i]);

			DrawTerrain(gridSize);
			DrawSphere({0, 0, 0}, 0.3f, RED);

			EndMode3D();

			EndScissorMode();

			// Draw border (highlight active)
			DrawRectangleLinesEx(vp, 2, (i == activeCamera) ? YELLOW : DARKGRAY);
		}

		DrawText("Click a viewport to control that camera", 10, 10, 20, WHITE);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
