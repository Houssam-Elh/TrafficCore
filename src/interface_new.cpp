#include "interface_new.h"
#include <algorithm> // For fmaxf, fminf

// =============================================================================
//  BUTTON IMPLEMENTATION
// =============================================================================

Button::Button(float x, float y, float w, float h, const std::string& txt, Color normal, Color hover)
    : rect{x, y, w, h}, text(txt), normalColor(normal), 
      hoverColor(hover), isHovered(false) {}

bool Button::IsClicked() {
    Vector2 mouse = GetMousePosition();
    isHovered = CheckCollisionPointRec(mouse, rect);

    return isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void Button::Draw() {
    Color currentColor = isHovered ? hoverColor : normalColor;
    
    // Shadow effect
    DrawRectangle(rect.x + 4, rect.y + 4, rect.width, rect.height, Fade(BLACK, 0.3f));
    
    // Button body
    DrawRectangleRounded(rect, 0.2f, 10, currentColor);
    DrawRectangleRoundedLines(rect, 0.2f, 10, isHovered ? WHITE : LIGHTGRAY);
    
    // Text
    int textWidth = MeasureText(text.c_str(), 24);
    DrawText(text.c_str(), 
            (int)(rect.x + (rect.width - textWidth) / 2),
            (int)(rect.y + (rect.height - 24) / 2),
            24, WHITE);
}

// =============================================================================
//  SLIDER IMPLEMENTATION
// =============================================================================

Slider::Slider(float x, float y, float w, const std::string& lbl, float minV, float maxV, float current)
    : rect{x, y, w, 20}, label(lbl), minValue(minV), 
      maxValue(maxV), currentValue(current), isDragging(false) {}

void Slider::Update() {
    Vector2 mouse = GetMousePosition();
    
    // Hitbox covers the whole slider bar + padding (easier to click)
    Rectangle hitRect = {
        rect.x - 5, rect.y - 10, 
        rect.width + 10, rect.height + 20
    };
    
    // START DRAGGING: Click anywhere on the bar
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, hitRect)) {
        isDragging = true;
        // Update immediately on click
        float ratio = (mouse.x - rect.x) / rect.width;
        ratio = fmaxf(0.0f, fminf(1.0f, ratio));
        currentValue = minValue + ratio * (maxValue - minValue);
    }
    
    // STOP DRAGGING
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        isDragging = false;
    }
    
    // UPDATE WHILE DRAGGING
    if (isDragging) {
        float ratio = (mouse.x - rect.x) / rect.width;
        ratio = fmaxf(0.0f, fminf(1.0f, ratio));
        currentValue = minValue + ratio * (maxValue - minValue);
    }
}

void Slider::Draw() {
    // Label
    DrawText(label.c_str(), (int)rect.x, (int)rect.y - 30, 20, WHITE);
    
    // Track
    DrawRectangleRec(rect, DARKGRAY);
    
    // Filled track
    float fillWidth = ((currentValue - minValue) / (maxValue - minValue)) * rect.width;
    DrawRectangle((int)rect.x, (int)rect.y, (int)fillWidth, (int)rect.height, SKYBLUE);
    
    // Handle
    float handleX = rect.x + fillWidth - 10;
    DrawRectangle((int)handleX, (int)rect.y - 5, 20, 30, WHITE);
    DrawRectangleLines((int)handleX, (int)rect.y - 5, 20, 30, BLACK);
    
    // Value display
    char valueText[32];
    if (maxValue > 10.0f) sprintf(valueText, "%.0f", currentValue); // Integer display for count
    else sprintf(valueText, "%.1fx", currentValue); // Decimal for speed
    
    DrawText(valueText, (int)(rect.x + rect.width + 20), (int)rect.y - 5, 20, WHITE);
}

// =============================================================================
//  INTERFACE CLASS IMPLEMENTATION
// =============================================================================

TrafficInterface::TrafficInterface()
    : currentState(STATE_MENU),
      animTimer(0.0f),
      startBtn(490, 330, 300, 60, "DEMARRER"), 
      exitBtn(490, 405, 300, 60, "QUITTER"),
      backBtn(50, 650, 200, 50, "< RETOUR"),
      launchBtn(960, 650, 270, 50, "LANCER SIMULATION"), // Changed Text & Width
      speedSlider(400, 230, 400, "Vitesse Simulation", 0.5f, 3.0f, 1.0f),
      shouldStartSimulation(false),
      shouldExit(false)
{}

void TrafficInterface::SyncFromConfig() {
    speedSlider.currentValue = globalConfig.simulationSpeed;
}

// Helper for Small Buttons
bool TrafficInterface::DrawMiniButton(float x, float y, const char* text, bool disabled) {
    Rectangle rect = {x, y, 30, 30};
    Color baseColor = disabled ? (Color){80, 80, 80, 255} : (Color){200, 200, 200, 255};
    Color textColor = disabled ? GRAY : BLACK;
    
    DrawRectangleRec(rect, baseColor);
    DrawRectangleLinesEx(rect, 2, GRAY);
    DrawText(text, (int)x + (30 - MeasureText(text, 20))/2, (int)y + 5, 20, textColor);
    
    if (disabled) return false;
    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, rect)) {
        DrawRectangleLinesEx(rect, 2, WHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return true;
    }
    return false;
}

void TrafficInterface::DrawAnimatedBackground() {
    DrawRectangleGradientV(0, 0, 1280, 720, 
                          (Color){20, 30, 50, 255}, 
                          (Color){40, 50, 80, 255});

    animTimer += GetFrameTime();
    float offset = fmodf(animTimer * 20.0f, 40.0f);
    
    for (int x = -40; x < 1280; x += 40) DrawLine(x + (int)offset, 0, x + (int)offset, 720, Fade(SKYBLUE, 0.1f));
    for (int y = -40; y < 720; y += 40) DrawLine(0, y + (int)offset, 1280, y + (int)offset, Fade(SKYBLUE, 0.1f));

    // Floating particles
    for (int i = 0; i < 20; i++) {
        float x = 100 + i * 50 + sinf(animTimer + i) * 30;
        float y = 100 + sinf(animTimer * 0.5f + i * 0.5f) * 200;
        DrawCircle((int)x, (int)y, 3, Fade(SKYBLUE, 0.3f));
    }
}

void TrafficInterface::DrawMainMenu() {
    DrawAnimatedBackground();
    
    const char* title = "TRAFFIC CITY";
    int titleSize = 80;
    int titleWidth = MeasureText(title, titleSize);
    int titleX = (1280 - titleWidth) / 2;
    int titleY = 120;
    
    for (int i = 0; i < 3; i++) {
        DrawText(title, titleX - i, titleY - i, titleSize, 
                Fade(SKYBLUE, 0.3f - i * 0.1f));
    }
    DrawText(title, titleX, titleY, titleSize, WHITE);
    
    const char* subtitle = "Simulation de Trafic Urbain";
    int subtitleWidth = MeasureText(subtitle, 24);
    DrawText(subtitle, (1280 - subtitleWidth) / 2, titleY + 100, 24, LIGHTGRAY);
    
    float carY = titleY + 150 + sinf(animTimer * 2.0f) * 10;
    DrawRectangle(640 - 40, (int)carY, 80, 30, GOLD);
    DrawRectangle(640 - 30, (int)carY - 10, 60, 15, SKYBLUE);
    DrawCircle(640 - 20, (int)carY + 30, 8, BLACK);
    DrawCircle(640 + 20, (int)carY + 30, 8, BLACK);

    startBtn.Draw();
    exitBtn.Draw();
    
    DrawText("v2.0 Final", 20, 690, 16, GRAY);

    const char* hint = "Utilisez la souris pour naviguer";
    int hintWidth = MeasureText(hint, 16);
    DrawText(hint, (1280 - hintWidth) / 2, 680, 16, GRAY);
}

// Settings Menu - Directly Edits Global Config
void TrafficInterface::DrawSettingsMenu() {
    DrawAnimatedBackground();
    const char* title = "CONFIGURATION";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, (1280 - titleWidth) / 2, 40, 60, WHITE);

    DrawRectangle(300, 110, 680, 520, Fade(BLACK, 0.6f));
    DrawRectangleLines(300, 110, 680, 520, SKYBLUE);
    
    // 1. Calculate Totals FIRST
    int currentTotal = 0;
    for (const auto& vc : globalConfig.vehicleConfigs) currentTotal += vc.count;

    // 2. Max Vehicles Controls (Now using Buttons!)
    int startX = 400;
    int startY = 150;

    // Label on the left
    DrawText("Max Limit:", startX, startY, 20, WHITE);
    
    // Safety Limits
    bool canDecrease = (globalConfig.maxVehicles > currentTotal);
    bool canIncrease = (globalConfig.maxVehicles < 100);
    
    // [-] Button
    if (DrawMiniButton((float)startX + 250, (float)startY - 5, "-", !canDecrease)) {
        if (canDecrease) globalConfig.maxVehicles--;
    }
    
    // Number 
    DrawText(TextFormat("%d", globalConfig.maxVehicles), startX + 300, startY, 20, WHITE);

    // [+] Button
    if (DrawMiniButton((float)startX + 350, (float)startY - 5, "+", !canIncrease)) {
        if (canIncrease) globalConfig.maxVehicles++;
    }

    // 3. Speed Slider
    speedSlider.Update();
    speedSlider.Draw();
    globalConfig.simulationSpeed = speedSlider.currentValue;

    // 4. Vehicle List (Same as In-Game)
    bool isFull = (currentTotal >= globalConfig.maxVehicles);
    startY = 300;
    Color limitColor = isFull ? RED : GREEN;
    DrawText(TextFormat("Utilisation: %d / %d", currentTotal, globalConfig.maxVehicles), startX, startY - 30, 20, limitColor);

    for (auto& vConfig : globalConfig.vehicleConfigs) {
        DrawText(TextFormat("%s:", vConfig.type.c_str()), startX, startY + 5, 20, WHITE);
        if (DrawMiniButton((float)startX + 250, (float)startY, "-", vConfig.count <= 0)) {
            if (vConfig.count > 0) vConfig.count--;
        }
        DrawText(TextFormat("%d", vConfig.count), startX + 300, startY + 5, 20, WHITE);
        if (DrawMiniButton((float)startX + 350, (float)startY, "+", isFull)) {
            if (!isFull) vConfig.count++;
        }
        startY += 40;
    }
    
    backBtn.Draw();
    launchBtn.Draw();
}

void TrafficInterface::DrawLoadingScreen() {
    DrawAnimatedBackground();

    const char* text = "CHARGEMENT DE LA VILLE...";
    int textWidth = MeasureText(text, 40);
    DrawText(text, (1280 - textWidth) / 2, 300, 40, WHITE);
    
    float progress = fmodf(animTimer * 0.5f, 1.0f);
    DrawRectangle(340, 380, 600, 30, DARKGRAY);
    DrawRectangle(340, 380, (int)(600 * progress), 30, SKYBLUE);
    DrawRectangleLines(340, 380, 600, 30, WHITE);

    float angle = animTimer * 180.0f;
    DrawCircleSector((Vector2){640, 480}, 30, angle, angle + 60, 20, SKYBLUE);
}

void TrafficInterface::DrawPauseOverlay() {
    DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.6f));

    const char* text = "PAUSE";
    int textWidth = MeasureText(text, 80);
    DrawText(text, (1280 - textWidth) / 2, 280, 80, WHITE);
}

void TrafficInterface::Update() {
    switch (currentState) {
        case STATE_MENU:
            if (startBtn.IsClicked()) currentState = STATE_SETTINGS;
            if (exitBtn.IsClicked()) shouldExit = true;
            break;
            
        case STATE_SETTINGS:
            if (backBtn.IsClicked()) currentState = STATE_MENU;
            if (launchBtn.IsClicked()) {
                shouldStartSimulation = true; 
                // We stay in this state for 1 frame until main.cpp takes over with the loading screen
            }
            break;
            
        case STATE_SIMULATION: break;
        case STATE_PAUSED: break;
    }
}

void TrafficInterface::Draw() {
    switch (currentState) {
        case STATE_MENU: DrawMainMenu(); break;
        case STATE_SETTINGS: DrawSettingsMenu(); break;
        case STATE_PAUSED: DrawPauseOverlay(); break;
        default: break;
    }
}

GameState TrafficInterface::GetState() const { return currentState; }
void TrafficInterface::SetState(GameState state) { currentState = state; }
bool TrafficInterface::IsInSimulation() const { return currentState == STATE_SIMULATION; }