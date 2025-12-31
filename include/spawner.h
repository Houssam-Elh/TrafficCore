#ifndef SPAWNER_H
#define SPAWNER_H

#include <vector>
#include <string>
#include <memory>
#include "vehicle.h"
#include "roadgraph.h"
#include "config.h"

// Helper struct for the queue
struct QueuedVehicle {
    std::string type;
    int startNodeId;
};

class VehicleSpawner {
private:
    std::vector<QueuedVehicle> spawnQueue;

    // The "Factory" helper function
    std::unique_ptr<Vehicle> CreateVehicle(const std::string& type, Vector3 pos, int targetNodeId);

public:
    VehicleSpawner();

    // Reloads the queue from Global Config
    void LoadFromConfig();

    // Checks timers and adds new vehicles to the list if possible
    void Update(RoadGraph& graph, std::vector<std::unique_ptr<Vehicle>>& vehicles);
    
    // Clears the queue
    void Clear();
};

#endif