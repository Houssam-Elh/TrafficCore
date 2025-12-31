#include "ingame_menu.h"
#include <algorithm> 

// Helper to draw pixel-style buttons
bool InGameMenu::DrawPixelButton(int x, int y, const char* text, int w, int h, bool disabled, Color btnColorOverride) {
    Rectangle btnRect = { (float)x, (float)y, (float)w, (float)h };
    
    Color btnColor = disabled ? (Color){100, 100, 100, 255} : btnColorOverride;
    Color textColor = disabled ? GRAY : BLACK;

    DrawRectangleRec(btnRect, btnColor);
    DrawRectangleLinesEx(btnRect, 2, GRAY);
    
    int textW = MeasureText(text, 20);
    DrawText(text, x + (w - textW)/2, y + (h - 20)/2, 20, textColor);

    if (disabled) return false;

    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, btnRect)) { 
        DrawRectangleLinesEx(btnRect, 2, WHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return true;
    }
    return false;
}

void InGameMenu::Draw(TrafficInterface& interface, Simulation& simulation, bool& gameStarted) {
    if (!isVisible) return;

    // Darken background
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 150});

    int panelW = 450;
    int panelH = 650;
    int panelX = (GetScreenWidth() - panelW) / 2;
    int panelY = (GetScreenHeight() - panelH) / 2;

    // Draw Panel
    DrawRectangle(panelX, panelY, panelW, panelH, MENU_BG_COLOR);
    DrawRectangleLinesEx({(float)panelX, (float)panelY, (float)panelW, (float)panelH}, 2, WHITE);
    
    int currentY = panelY + 20;
    int contentX = panelX + 30;

    // --- HEADER ---
    DrawText("PAUSE & SETTINGS", contentX, currentY, 20, MENU_TEXT_COLOR);

    // --- X BUTTON --- (Close)
    if (DrawPixelButton(panelX + panelW - 45, currentY - 5, "X")) {
        isVisible = false;
        interface.SetState(STATE_SIMULATION); 
    }
    currentY += 60;

    // --- CALCULATE CONFIG TOTAL ---
    int configTotal = 0;
    for (const auto& vc : globalConfig.vehicleConfigs) {
        configTotal += vc.count;
    }

    // --- 1. MAX VEHICLES ---
    DrawText(TextFormat("Max Limit: %d", globalConfig.maxVehicles), contentX, currentY, 18, MENU_TEXT_COLOR);
    
    // Safety Limits
    bool canDecreaseMax = (globalConfig.maxVehicles > configTotal);
    bool canIncreaseMax = (globalConfig.maxVehicles < 100);

    // --- +/- BUTTONS ---
    if (DrawPixelButton(panelX + 260, currentY - 5, "-", 30, 30, !canDecreaseMax)) {
        if (canDecreaseMax) globalConfig.maxVehicles--;
    }
    if (DrawPixelButton(panelX + 310, currentY - 5, "+", 30, 30, !canIncreaseMax)) {
        if (canIncreaseMax) globalConfig.maxVehicles++;
    }
    currentY += 50;

    // --- 2. SPEED SLIDER ---
    DrawText(TextFormat("Speed: %.1fx", globalConfig.simulationSpeed), contentX, currentY, 18, MENU_TEXT_COLOR);

    Rectangle sliderRect = { (float)panelX + 220, (float)currentY + 5, 150, 20 };
    DrawRectangleRec(sliderRect, GRAY);
    
    float ratio = (globalConfig.simulationSpeed - 0.5f) / (3.0f - 0.5f); 
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    
    DrawRectangle(sliderRect.x, sliderRect.y, ratio * sliderRect.width, sliderRect.height, MENU_HIGHLIGHT);

    // Slider Logic
    Vector2 mouse = GetMousePosition();
    Rectangle hitRect = { sliderRect.x - 5, sliderRect.y - 10, sliderRect.width + 10, sliderRect.height + 20 };

    if (CheckCollisionPointRec(mouse, hitRect)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            float newRatio = (mouse.x - sliderRect.x) / sliderRect.width;
            if (newRatio < 0.0f) newRatio = 0.0f;
            if (newRatio > 1.0f) newRatio = 1.0f;
            
            globalConfig.simulationSpeed = 0.5f + (newRatio * (3.0f - 0.5f));

            // Min speed clamp (0.5x)
            if (globalConfig.simulationSpeed < 0.5f) globalConfig.simulationSpeed = 0.5f;
        }
    }
    currentY += 50;

    // --- 3. VEHICLE LIST ---
    Color limitColor = (configTotal >= globalConfig.maxVehicles) ? RED : GREEN;
    DrawText(TextFormat("Usage: %d / %d", configTotal, globalConfig.maxVehicles), contentX, currentY, 18, limitColor);
    currentY += 35;

    bool isFull = (configTotal >= globalConfig.maxVehicles);

    for (auto& vConfig : globalConfig.vehicleConfigs) {
        DrawText(TextFormat("%s:", vConfig.type.c_str()), contentX + 20, currentY + 5, 18, MENU_TEXT_COLOR);
        
        // Decrease Count
        if (DrawPixelButton(panelX + 260, currentY, "-")) {
            if (vConfig.count > 0) vConfig.count--;
        }
        
        DrawText(TextFormat("%d", vConfig.count), panelX + 310, currentY + 5, 18, MENU_TEXT_COLOR);

        // Increase Count (Only if not full)
        if (DrawPixelButton(panelX + 360, currentY, "+", 30, 30, isFull)) {
            if (!isFull) vConfig.count++;
        }
        currentY += 40;
    }
    currentY += 20;

    // --- APPLY BUTTON ---
    if (DrawPixelButton(panelX + (panelW - 260)/2, currentY, "APPLY & RESUME", 260, 50)) {
        simulation.ApplyConfiguration(); // Reloads the simulation
        isVisible = false;
        interface.SetState(STATE_SIMULATION);
    }
    currentY += 60; 

    // --- QUIT BUTTON ---
    if (DrawPixelButton(panelX + (panelW - 260)/2, currentY, "MAIN MENU", 260, 50, false, (Color){220, 100, 100, 255})) {
        isVisible = false;
        interface.SetState(STATE_MENU);
        interface.shouldStartSimulation = false;
        gameStarted = false;
        simulation.Clear();
    }
}