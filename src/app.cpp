#include "app.h"
#include <iostream>

App::App() {
    // 1. Window & System Setup
    InitWindow(SimulationConfig::SCREEN_WIDTH, SimulationConfig::SCREEN_HEIGHT, "Traffic Core Simulator");
    SetExitKey(KEY_NULL); // Disable default ESC
    SetTargetFPS(60);

    // 2. Camera Setup
    camera = { 0 };
    camera.position = (Vector3){ 130.0f, 100.0f, 130.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // 3. Module Initialization
    globalConfig = GetDefaultConfig();
    simulation.Init();
    simulation.ApplyConfiguration();
    interface.SyncFromConfig();

    // 4. Initial State
    gameStarted = false;
    loadingTimer = 0.0f;
    showDebugNodes = true;
}

App::~App() {
    CloseWindow();
}

void App::Run() {
    // The Main Loop
    while (!WindowShouldClose()) {
        Update();
        Draw();
        
        // Check if the exit button was pressed in the menu
        if (interface.shouldExit) break; 
    }
}

void App::Update() {
    interface.Update();

    // Transition Logic: Menu -> Loading -> Game
    if (interface.shouldStartSimulation && !gameStarted) {
        loadingTimer += GetFrameTime();
        if (loadingTimer > 3.0f) {
            simulation.ApplyConfiguration();
            interface.SetState(STATE_SIMULATION);
            gameStarted = true;
            loadingTimer = 0.0f;
        }
    }

    if (gameStarted) {
        // [P] Toggle Pause
        if (IsKeyPressed(KEY_P)) {
            pauseMenu.isVisible = !pauseMenu.isVisible;
            interface.SetState(pauseMenu.isVisible ? STATE_PAUSED : STATE_SIMULATION);
        }

        // [ESC] Return to Main Menu
        if (IsKeyPressed(KEY_ESCAPE)) {
            pauseMenu.isVisible = false;
            interface.SetState(STATE_MENU);
            interface.shouldStartSimulation = false;
            gameStarted = false;
            simulation.Clear();
        }

        // [N] Toggle Debug Nodes
        if (IsKeyPressed(KEY_N)) showDebugNodes = !showDebugNodes;

        // Camera Controls (only if not paused)
        if (!pauseMenu.isVisible) {
            float dt = GetFrameTime() * globalConfig.simulationSpeed;
            if (IsKeyDown(KEY_A)) camera.position.x -= 30 * dt;
            if (IsKeyDown(KEY_D)) camera.position.x += 30 * dt;
            if (IsKeyDown(KEY_W)) camera.position.z -= 30 * dt;
            if (IsKeyDown(KEY_S)) camera.position.z += 30 * dt;
        }

        // Simulation Update
        if (interface.IsInSimulation()) {
            float dt = GetFrameTime() * globalConfig.simulationSpeed;
            simulation.Update(dt, camera);
        }
    }
}

void App::Draw() {
    BeginDrawing();
        ClearBackground(RAYWHITE);

        if (interface.IsInSimulation() || interface.GetState() == STATE_PAUSED) {
            // 1. Draw 3D World
            BeginMode3D(camera);
                simulation.Draw3D(showDebugNodes); // No camera needed here
            EndMode3D();

            // 2. Draw Overlays (IDs, HUD, Menus)
            simulation.DrawOverlay(showDebugNodes, camera); // Camera needed for text projection

            // HUD
            DrawText("Traffic Core Simulator", 10, 10, 20, DARKGRAY);
            if (!pauseMenu.isVisible) {
                DrawText("- [P] : Settings", 10, 35, 20, DARKGRAY);
                DrawText("- [ESC] : Main Menu", 10, 60, 20, DARKGRAY);
                DrawText("- [N] : Show Nodes", 10, 85, 20, DARKGRAY);
                DrawText("- [WASD] : Move Camera", 10, 110, 20, DARKGRAY);
                DrawText("- Click Car : Force Move", 10, 135, 20, DARKGRAY);
                DrawText(TextFormat("- Vehicles: %d", simulation.GetVehicleCount()), 10, 160, 20, DARKGRAY);
            }

            // In-Game Menu
            pauseMenu.Draw(interface, simulation, gameStarted);
        }
        else {
            // Main Menu & Loading Screen
            interface.Draw();
            if (interface.shouldStartSimulation && !gameStarted) {
                interface.DrawLoadingScreen();
            }
        }

    EndDrawing();
}