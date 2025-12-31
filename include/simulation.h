#ifndef SIMULATION_H
#define SIMULATION_H

#include "raylib.h"
#include <vector>
#include <memory>

#include "roadgraph.h"
#include "vehicle.h"
#include "traffic_manager.h"
#include "spawner.h"

class Simulation {
private:
    RoadGraph roadGraph;
    TrafficManager trafficMgr;
    VehicleSpawner spawner;
    std::vector<std::unique_ptr<Vehicle>> vehicles;

public:
    Simulation();
    void Init();
    void ApplyConfiguration();
    void Update(float dt, Camera3D camera);
    void Draw3D(bool showDebugNodes);
    void DrawOverlay(bool showDebugNodes, Camera3D camera);
    int GetVehicleCount() const;
    void Clear();
};

#endif