#ifndef APP_H
#define APP_H

#include "raylib.h"
#include "simulation.h"
#include "interface_new.h"
#include "ingame_menu.h"
#include "config.h"

class App {
private:
    // Core Modules
    Camera3D camera;
    Simulation simulation;
    TrafficInterface interface;
    InGameMenu pauseMenu;

    // State Variables
    bool gameStarted;
    float loadingTimer;
    bool showDebugNodes;

    // Private helpers
    void Update();
    void Draw();

public:
    App();  // Constructor initializes Window & Modules
    ~App(); // Destructor closes Window
    
    // The only function main() needs to call
    void Run();
};

#endif