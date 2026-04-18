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

// ---------------- Agent ----------------
struct Agent {
	Vector3 position;
	float stepSize = 0.5f;
	bool converged = false;
};

// ---------------- Hill Climbing ----------------
void HillClimbStep(Agent& agent) {
	if (agent.converged) return;

	float bestHeight = GetHeight(agent.position.x, agent.position.z);
	Vector3 bestPos = agent.position;

	float step = agent.stepSize;

	Vector3 directions[4] = {{step, 0, 0}, {-step, 0, 0}, {0, 0, step}, {0, 0, -step}};

	for (auto& dir : directions) {
		Vector3 candidate = {agent.position.x + dir.x, 0, agent.position.z + dir.z};

		float h = GetHeight(candidate.x, candidate.z);

		if (h > bestHeight) {
			bestHeight = h;
			bestPos = candidate;
		}
	}

	if (bestPos.x == agent.position.x && bestPos.z == agent.position.z) {
		agent.converged = true;
	} else {
		agent.position = bestPos;
		agent.position.y = GetHeight(bestPos.x, bestPos.z);
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

	float hillTimer = 0.0f;
	float hillDelay = 0.2f; // seconds between steps
	const int gridSize = 50;

	// --- Agents ---
	std::vector<Agent> agents = {
	  {{-10, 0, -10}}, {{10, 0, -10}}, {{-10, 0, 10}}, {{10, 0, 10}}};

	// Initialize height
	for (auto& a : agents) {
		a.position.y = GetHeight(a.position.x, a.position.z);
	}

	// --- Cameras ---
	std::vector<CameraState> cameras(4);

	for (int i = 0; i < 4; i++) {
		cameras[i].cam.up = {0, 1, 0};
		cameras[i].cam.fovy = 60.0f;
		cameras[i].cam.projection = CAMERA_PERSPECTIVE;
	}

	int activeCamera = 0;

	// --- Render textures ---
	RenderTexture2D renderTex[4];

	int screenW = GetScreenWidth();
	int screenH = GetScreenHeight();

	for (int i = 0; i < 4; i++) {
		renderTex[i] = LoadRenderTexture(screenW / 2, screenH / 2);
	}

	while (!WindowShouldClose()) {
		hillTimer += GetFrameTime();

		if (hillTimer >= hillDelay) {
			hillTimer = 0.0f;

			for (int i = 0; i < 4; i++) {
				HillClimbStep(agents[i]);
			}
		}
		// -------- Resize handling --------
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

		// -------- Select viewport --------
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 m = GetMousePosition();
			for (int i = 0; i < 4; i++) {
				if (CheckCollisionPointRec(m, viewports[i])) {
					activeCamera = i;
				}
			}
		}
		if (IsKeyPressed(KEY_TAB)) {
			CameraState& camState = cameras[activeCamera];
			if (camState.mode == FREE) {
				camState.mode = LOCKED;
			} else if (camState.mode == LOCKED) {
				camState.mode = FREE;
			}
		}
		for (int i = 0; i < 4; i++) {
			CameraState& camState = cameras[i];
			Camera3D& cam = camState.cam;

			Vector3 target = agents[i].position;

			// -------- ACTIVE CAMERA --------
			if (i == activeCamera) {
				if (camState.mode == FREE) {
					UpdateCamera(&cam, CAMERA_FIRST_PERSON);
				} else {
					if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
						Vector2 delta = GetMouseDelta();
						camState.yaw -= delta.x * 0.3f;
						camState.pitch -= delta.y * 0.3f;
					}

					camState.pitch =
					  ClampValue(camState.pitch, -89.0f, 89.0f);

					camState.distance -= GetMouseWheelMove() * 2.0f;
					camState.distance =
					  ClampValue(camState.distance, 3.0f, 50.0f);

					float yawRad = DEG2RAD * camState.yaw;
					float pitchRad = DEG2RAD * camState.pitch;

					cam.position.x = target.x + camState.distance *
								cosf(pitchRad) *
								sinf(yawRad);
					cam.position.y =
					  target.y + camState.distance * sinf(pitchRad);
					cam.position.z = target.z + camState.distance *
								cosf(pitchRad) *
								cosf(yawRad);

					cam.target = target;
				}

			}
			// -------- INACTIVE CAMERAS --------
			else {
				// Always LOCKED (no input)
				float yawRad = DEG2RAD * camState.yaw;
				float pitchRad = DEG2RAD * camState.pitch;

				cam.position.x = target.x + camState.distance *
							cosf(pitchRad) * sinf(yawRad);
				cam.position.y =
				  target.y + camState.distance * sinf(pitchRad);
				cam.position.z = target.z + camState.distance *
							cosf(pitchRad) * cosf(yawRad);

				cam.target = target;
			}
		} // -------- Render each camera --------
		for (int i = 0; i < 4; i++) {
			BeginTextureMode(renderTex[i]);
			ClearBackground(BLACK);

			BeginMode3D(cameras[i].cam);

			DrawTerrain(gridSize);

			for (int j = 0; j < 4; j++) {
				Color color = (j == i) ? YELLOW : RED;
				DrawSphere(agents[j].position, 0.5f, color);
			}

			EndMode3D();
			EndTextureMode();
		}

		// -------- Draw to screen --------
		BeginDrawing();
		ClearBackground(BLACK);

		for (int i = 0; i < 4; i++) {
			DrawTexturePro(renderTex[i].texture,
				     {0, 0, (float)renderTex[i].texture.width,
				      -(float)renderTex[i].texture.height},
				     viewports[i], {0, 0}, 0.0f, WHITE);

			DrawRectangleLinesEx(viewports[i], 2,
					 (i == activeCamera) ? YELLOW : DARKGRAY);
		}

		DrawText("Click viewport | TAB: mode | RMB: rotate | Scroll: zoom", 20, 20, 20,
		         WHITE);

		EndDrawing();
	}

	for (int i = 0; i < 4; i++) {
		UnloadRenderTexture(renderTex[i]);
	}

	CloseWindow();
	return 0;
}
