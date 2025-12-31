#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <memory>

// Forward declaration to avoid circular includes
// (We only need to know 'Vehicle' exists here)
class Vehicle; 

// The TrafficManager handles collision avoidance and speed regulation.
// It acts as the "brain" for the simulation's traffic rules.
class TrafficManager {
private:
    // --- Configuration Parameters ---
    float startSlowingDist; // Distance at which a car starts braking
    float minSafeDist;      // Minimum distance to maintain when stopped
    float detectionRange;   // How far ahead the car "sees"

    // --- Internal Helper Functions ---
    
    // Calculates Euclidean distance between two 3D points
    float GetDistance(const Vector3& a, const Vector3& b);
    
    // Direction Check (Are we parallel?)
    // Returns true if two vectors point roughly in the same direction
    bool AreSameDirection(const Vector3& dir1, const Vector3& dir2);
    
    // Lane Check (Only for parallel cars)
    // Determines if another vehicle is directly ahead in the current lane
    bool IsInMyLane(Vehicle* me, Vehicle* other);

    // Linear Interpolation helper for smooth braking
    float Lerp(float start, float end, float amount);

public:
    // Constructor with default safety values
    TrafficManager(float slowDist = 12.0f, float detection = 30.0f);
    
    // Main Logic Loop:
    // Iterates through all vehicles to calculate and apply their target speeds
    // based on traffic conditions (cars ahead, intersections, etc.).
    void UpdateVehicles(std::vector<std::unique_ptr<Vehicle>>& vehicles);
};

#endif // TRAFFIC_MANAGER_H