#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"
#include <string>
#include <vector>

// Structure for a specific type of vehicle (e.g., "Car", 10 cars, start nodes...)
struct VehicleSpawnConfig {
    std::string type;
    int count;          // This will be editable in the menu
    std::vector<int> startNodes;
};

// Main Configuration Structure
struct SimulationConfig {
    int maxVehicles = 50;
    float simulationSpeed = 1.0f; // 1.0x = Normal, 2.0x = Fast
    
    // List of all vehicle groups
    std::vector<VehicleSpawnConfig> vehicleConfigs;
    
    // Constants (Screen size, etc.)
    static const int SCREEN_WIDTH = 1280;
    static const int SCREEN_HEIGHT = 720;
    
    // Vehicle Speeds
    static constexpr float CAR_SPEED = 15.0f;
    static constexpr float BUS_SPEED = 10.0f;
    static constexpr float TRUCK_SPEED = 8.0f;
    static constexpr float TAXI_SPEED = 18.0f;
    static constexpr float POLICE_SPEED = 22.0f;
    static constexpr float MOTORCYCLE_SPEED = 20.0f;
    static constexpr float ARRIVAL_THRESHOLD = 2.0f;
};

// Global config instance (declared here, defined in cpp)
extern SimulationConfig globalConfig; 

// Function to reset/load defaults
SimulationConfig GetDefaultConfig();

// Cet alias permet d'utiliser "CONFIG::CAR_SPEED" dans votre code
// au lieu de devoir écrire "SimulationConfig::CAR_SPEED"
using CONFIG = SimulationConfig;

#endif