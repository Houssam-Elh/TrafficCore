#include "simulation.h"
#include "basicmap.h"
#include <cmath> // Needed for fabs

Simulation::Simulation() : trafficMgr(20.0f, 50.0f) {} 

void Simulation::Init() {
    InitializeRoadNetwork(roadGraph);
}

void Simulation::ApplyConfiguration() {
    vehicles.clear();
    roadGraph.Clear();
    InitializeRoadNetwork(roadGraph);
    spawner.LoadFromConfig();
}

void Simulation::Clear() {
    vehicles.clear();
    spawner.Clear();
}

int Simulation::GetVehicleCount() const {
    return (int)vehicles.size();
}

void Simulation::Update(float dt, Camera3D camera) {
    // 1. Spawner
    spawner.Update(roadGraph, vehicles);

    // =========================================================
    //  INTERACTION: ADAPTIVE HITBOX & CLOSEST PICK
    // =========================================================
    Ray ray = GetMouseRay(GetMousePosition(), camera);
    
    Vehicle* hoveredVehicle = nullptr;
    float minHitDist = 9999.0f; // Track closest hit

    for (auto& v : vehicles) {
        // --- ADAPTIVE HITBOX MATH ---
        // We calculate how much space the car takes on X and Z axes based on its rotation.
        // Width is approx 2.5m for all cars. Length varies.
        float width = 2.5f;
        
        // Project length and width onto the axes
        // If facing X: SizeX = Length, SizeZ = Width
        // If facing Z: SizeX = Width,  SizeZ = Length
        // If 45 deg:   SizeX = Mix,    SizeZ = Mix
        float halfSizeX = (fabs(v->forward.x) * v->length + fabs(v->forward.z) * width) / 2.0f;
        float halfSizeZ = (fabs(v->forward.z) * v->length + fabs(v->forward.x) * width) / 2.0f;

        // Construct the rotating box
        BoundingBox box = {
            (Vector3){ v->position.x - halfSizeX, v->position.y, v->position.z - halfSizeZ },
            (Vector3){ v->position.x + halfSizeX, v->position.y + 2.5f, v->position.z + halfSizeZ }
        };

        // Check Raycast
        RayCollision collision = GetRayCollisionBox(ray, box);
        
        if (collision.hit) {
            // Only pick this car if it is closer than previous hits
            if (collision.distance < minHitDist) {
                minHitDist = collision.distance;
                hoveredVehicle = v.get();
            }
        }
    }
    
    // --- APPLY INTERACTION TO THE WINNER ---
    if (hoveredVehicle != nullptr) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            hoveredVehicle->forceMoveTimer = 2.0f;
        }
    }

    // =========================================================

    // 2. Traffic Logic
    trafficMgr.UpdateVehicles(vehicles);
    
    // 3. Physics
    for (auto &v : vehicles) {
        v->update(dt, roadGraph, vehicles); 
    }
}

void Simulation::Draw3D(bool showDebugNodes) {
    DrawBasicMap();
    if (showDebugNodes) roadGraph.DrawNodes();
    for (auto &v : vehicles) v->draw();
}

void Simulation::DrawOverlay(bool showDebugNodes, Camera3D camera) {
    if (showDebugNodes) roadGraph.DrawIdNodes(camera);
}