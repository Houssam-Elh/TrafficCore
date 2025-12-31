#include "config.h"

// Define the global config
SimulationConfig globalConfig;

SimulationConfig GetDefaultConfig() {
    SimulationConfig cfg;
    
    cfg.maxVehicles = 50;
    cfg.simulationSpeed = 1.0f;

    // --- 1. DECLARE YOUR SHARED LIST HERE ---
    // This list contains ALL the valid green "START" nodes from your map.
    std::vector<int> validStartNodes = {0, 1, 26, 27, 30, 31, 35, 50};

    // 1. CARS
    VehicleSpawnConfig car;
    car.type = "Car";
    car.count = 8;
    car.startNodes = validStartNodes;
    cfg.vehicleConfigs.push_back(car);

    // 2. BUSES
    VehicleSpawnConfig bus;
    bus.type = "Bus";
    bus.count = 3;
    bus.startNodes = validStartNodes;
    cfg.vehicleConfigs.push_back(bus);

    // 3. TRUCKS
    VehicleSpawnConfig truck;
    truck.type = "Truck";
    truck.count = 3;
    truck.startNodes = validStartNodes;
    cfg.vehicleConfigs.push_back(truck);

    // 4. TAXIS
    VehicleSpawnConfig taxi;
    taxi.type = "Taxi";
    taxi.count = 5;
    taxi.startNodes = validStartNodes;
    cfg.vehicleConfigs.push_back(taxi);

    // 5. POLICE
    VehicleSpawnConfig police;
    police.type = "Police";
    police.count = 2;
    police.startNodes = validStartNodes;
    cfg.vehicleConfigs.push_back(police);

    // 6. MOTORCYCLES
    VehicleSpawnConfig moto;
    moto.type = "Motorcycle";
    moto.count = 4;
    moto.startNodes = validStartNodes;
    cfg.vehicleConfigs.push_back(moto);

    return cfg;
}