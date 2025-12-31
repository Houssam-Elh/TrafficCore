#include "spawner.h"
#include "raymath.h" // For Vector3 operations

VehicleSpawner::VehicleSpawner() {}

void VehicleSpawner::LoadFromConfig() {
    spawnQueue.clear();
    for (const auto& cfg : globalConfig.vehicleConfigs) {
        for(int i = 0; i < cfg.count; i++) {
            if (cfg.startNodes.empty()) continue;
            int nodeId = cfg.startNodes[GetRandomValue(0, cfg.startNodes.size() - 1)];
            spawnQueue.push_back({cfg.type, nodeId});
        }
    }
}

void VehicleSpawner::Clear() {
    spawnQueue.clear();
}

std::unique_ptr<Vehicle> VehicleSpawner::CreateVehicle(const std::string& type, Vector3 pos, int target) {
    if (type == "Car") return std::make_unique<Car>(pos, target);
    if (type == "Bus") return std::make_unique<Bus>(pos, target);
    if (type == "Truck") return std::make_unique<Truck>(pos, target);
    if (type == "Taxi") return std::make_unique<Taxi>(pos, target);
    if (type == "Police") return std::make_unique<PoliceCar>(pos, target);
    if (type == "Motorcycle") return std::make_unique<Motorcycle>(pos, target);
    return nullptr;
}

void VehicleSpawner::Update(RoadGraph& graph, std::vector<std::unique_ptr<Vehicle>>& vehicles) {
    for (auto it = spawnQueue.begin(); it != spawnQueue.end(); ) {
        Node &n = graph.GetNode(it->startNodeId);

        // --- 1. SMART SAFETY CHECK ---
        // We check physical space.
        // We assume the node is blocked until proven otherwise.
        bool isBlocked = false;

        for (const auto& v : vehicles) {
            // Distance Check:
            // 8.0f ensures a natural "following distance" gap.
            if (Vector3Distance(v->position, n.pos) < 8.0f) {
                isBlocked = true;
                break;
            }
        }

        // --- 2. SPAWN LOGIC ---
        // Only spawn if the spawn point is physically clear (!isBlocked)
        if (!isBlocked) {

            if (!n.nextNodes.empty()) {
                Vector3 pos = n.pos;
                int target = n.nextNodes[0];

                // 1. Create the specific vehicle
                auto newVehicle = CreateVehicle(it->type, pos, target);
                
                // 2. Fix Orientation
                if (newVehicle) {
                    Vector3 targetPos = graph.GetNode(target).pos;
                    Vector3 dir = Vector3Subtract(targetPos, pos);
                    newVehicle->forward = Vector3Normalize(dir);
                    
                    // Add to the main simulation list
                    vehicles.push_back(std::move(newVehicle));
                }
            }

            // Success: Remove from queue
            it = spawnQueue.erase(it);
        } else {
            // Blocked: Try next one later
            ++it;
        }
    }
}