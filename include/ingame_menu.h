#ifndef INGAME_MENU_H
#define INGAME_MENU_H

#include "raylib.h"
#include "config.h"
#include "interface_new.h"
#include "simulation.h" 

class InGameMenu {
private:
    Color MENU_BG_COLOR = {50, 50, 50, 255};
    Color MENU_BTN_COLOR = {200, 200, 200, 255};
    Color MENU_TEXT_COLOR = WHITE;
    Color MENU_HIGHLIGHT = {100, 200, 255, 255};

    bool DrawPixelButton(int x, int y, const char* text, int w = 30, int h = 30, bool disabled = false, Color btnColorOverride = {200, 200, 200, 255});

public:
    bool isVisible = false;

    // Returns true if the game should be reset/stopped
    void Draw(TrafficInterface& interface, Simulation& simulation, bool& gameStarted);
};

#endif