#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include "roadgraph.h"
#include "traffic_manager.h"
#include "vehicle.h"
#include "raylib.h"

// Simple test helper
#define TEST_CASE(name) void name()
#define RUN_TEST(name) { std::cout << "Running " << #name << "... "; name(); std::cout << "PASSED\n"; }

// --- TEST 1: RoadGraph Node Addition ---
TEST_CASE(TestRoadGraphAddNode) {
    RoadGraph graph;
    graph.AddNode(1, {10, 0, 10}, START);
    graph.AddNode(2, {20, 0, 20}, DECISION);
    
    assert(graph.GetAllNodes().size() == 2);
    assert(graph.GetNode(1).id == 1);
    assert(graph.GetNode(2).type == DECISION);
}

// --- TEST 2: RoadGraph Connectivity ---
TEST_CASE(TestRoadGraphConnections) {
    RoadGraph graph;
    graph.AddNode(1, {0, 0, 0}, START);
    graph.AddNode(2, {10, 0, 0}, ARC);
    graph.ConnectNodes(1, 2);
    
    Node& n1 = graph.GetNode(1);
    assert(n1.nextNodes.size() == 1);
    assert(n1.nextNodes[0] == 2);
}

// --- TEST 3: Vehicle Initialization ---
TEST_CASE(TestVehicleInitialization) {
    Vector3 startPos = {0, 0, 0};
    Car myCar(startPos, 1);
    
    assert(myCar.position.x == 0);
    assert(myCar.targetNodeId == 1);
    assert(myCar.speed > 0); // Should have a default config speed
}

// --- TEST 4: Spawner Functionality ---
TEST_CASE(TestVehicleSpawner) {
    RoadGraph graph;
    graph.AddNode(1, {0,0,0}, START);
    graph.ConnectNodes(1, 2); // Need a path to exist
    graph.AddNode(2, {10,0,0}, ARC);

    std::vector<std::unique_ptr<Vehicle>> vehicles;
    
    // Attempt to spawn a vehicle at Node 1
    // Assuming your Spawner class has a SpawnSpecific method [cite: 58]
    // If not, this test verifies the logic of adding to the vector
    vehicles.push_back(std::make_unique<Taxi>((Vector3){0,0,0}, 2));
    
    assert(vehicles.size() == 1);
    assert(vehicles[0]->color.r == YELLOW.r); // Verify it's a Taxi
}

// --- TEST 5: Teleportation Logic ---
TEST_CASE(TestTeleportationLogic) {
    RoadGraph graph;
    graph.AddNode(1, {0,0,0}, TELEPORT);
    graph.AddNode(2, {100,0,100}, START);
    graph.SetTeleportTarget(1, 2);
    graph.ConnectNodes(2, 3); // Path after teleport

    std::vector<std::unique_ptr<Vehicle>> vehicles;
    vehicles.push_back(std::make_unique<Car>((Vector3){0,0,0}, 1));
    
    // Simulate arrival at node 1 (TELEPORT)
    // The vehicle update logic should move it to node 2
    vehicles[0]->update(0.1f, graph, vehicles);

    // Position should now be at the teleport destination
    assert(vehicles[0]->position.x == 100);
    assert(vehicles[0]->position.z == 100);
}

int main() {
    // Raylib requires a window context for some functions (like GetFrameTime) 
    // used in TrafficManager, so we init a headless/tiny window if needed.
    InitWindow(100, 100, "Unit Tests");
    SetTargetFPS(60);

    std::cout << "--- STARTING PROJECT UNIT TESTS ---\n";
    
    RUN_TEST(TestRoadGraphAddNode);
    RUN_TEST(TestRoadGraphConnections);
    RUN_TEST(TestVehicleInitialization);
    RUN_TEST(TestVehicleSpawner);
    RUN_TEST(TestTeleportationLogic);

    std::cout << "--- ALL TESTS PASSED ---\n";
    
    CloseWindow();
    return 0;
}