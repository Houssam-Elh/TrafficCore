#ifndef INTERFACE_H
#define INTERFACE_H

#include "raylib.h"
#include <string>
#include <cmath>
#include <cstdio>
#include <vector>

#include "config.h"

// =============================================================================
//  ENUMS & STRUCTURES
// =============================================================================

enum GameState {
    STATE_MENU,
    STATE_SETTINGS,
    STATE_SIMULATION,
    STATE_PAUSED
};

// --- BUTTON STRUCT ---
struct Button {
    Rectangle rect;
    std::string text;
    Color normalColor;
    Color hoverColor;
    bool isHovered;
    
    // Constructor
    Button(float x, float y, float w, float h, const std::string& txt, 
           Color normal = DARKGRAY, Color hover = GRAY);
    
    // Core Functions
    bool IsClicked();
    void Draw();
};

// --- SLIDER STRUCT ---
struct Slider {
    Rectangle rect;
    std::string label;
    float minValue, maxValue;
    float currentValue;
    bool isDragging;
    
    // Constructor
    Slider(float x, float y, float w, const std::string& lbl, 
           float minV, float maxV, float current);
    
    // Core Functions
    void Update();
    void Draw();
};

// =============================================================================
//  INTERFACE CLASS
// =============================================================================

class TrafficInterface {
private:
    GameState currentState;
    float animTimer;
    
    // Buttons
    Button startBtn;
    Button exitBtn;
    Button backBtn;
    Button launchBtn;
    
    // Sliders
    Slider speedSlider;

    // Helper for Small Buttons (Internal Use Only)
    bool DrawMiniButton(float x, float y, const char* text, bool disabled = false);

public:
    // Public Flags
    bool shouldStartSimulation;
    bool shouldExit;
    
    // Constructor
    TrafficInterface();

    // Sync Sliders with Global Config (Call this on startup!)
    void SyncFromConfig();
    
    // --- DRAWING ROUTINES ---
    void DrawAnimatedBackground();
    void DrawMainMenu();
    void DrawSettingsMenu();   // Directly Edits Global Config
    void DrawLoadingScreen();
    void DrawPauseOverlay();
    
    // --- MAIN LOOP METHODS ---
    void Update();
    void Draw();
    
    // --- GETTERS & SETTERS ---
    GameState GetState() const;
    void SetState(GameState state);
    bool IsInSimulation() const;
};

#endif