#include "traffic_manager.h"
#include "vehicle.h"
#include <cmath>
#include <algorithm>
#include "raymath.h" 

// =============================================================================
//  HELPER FUNCTIONS
// =============================================================================

float TrafficManager::GetDistance(const Vector3& a, const Vector3& b) {
    return Vector3Distance(a, b);
}

bool TrafficManager::AreSameDirection(const Vector3& dir1, const Vector3& dir2) {
    float dotProduct = dir1.x * dir2.x + dir1.z * dir2.z;
    return dotProduct > 0.7f;
}

bool TrafficManager::IsInMyLane(Vehicle* me, Vehicle* other) {
    Vector3 toOther = Vector3Subtract(other->position, me->position);

    float forwardDist = Vector3DotProduct(toOther, me->forward);
    if (forwardDist < 0) return false; 

    Vector3 right = { -me->forward.z, 0, me->forward.x };
    float sideDist = Vector3DotProduct(toOther, right);

    return fabs(sideDist) < 1.8f; 
}

float TrafficManager::Lerp(float start, float end, float amount) {
    return start + amount * (end - start);
}

// =============================================================================
//  CONSTRUCTOR
// =============================================================================

TrafficManager::TrafficManager(float slowDist, float detection)
    : startSlowingDist(slowDist), detectionRange(detection) 
{
    minSafeDist = 4.0f; 
}

// =============================================================================
//  MAIN UPDATE LOOP
// =============================================================================

void TrafficManager::UpdateVehicles(std::vector<std::unique_ptr<Vehicle>>& vehicles) {
    float dt = GetFrameTime();

    for (size_t i = 0; i < vehicles.size(); i++) {
        Vehicle* current = vehicles[i].get();
        if (current->finished) continue;

        if (current->forceMoveTimer > 0.0f) {
            current->forceMoveTimer -= dt;
        }
        
        float targetSpeed = current->desiredSpeed;
        bool emergencyStop = false; 
        
        float closestGap = 9999.0f;
        Vehicle* closestVehicle = nullptr;
        bool followMode = false;

        float dynamicDetectionRange = detectionRange + (current->speed * 2.0f);
        float dynamicSlowingDist = startSlowingDist + (current->speed * 1.5f);

        // --- SCAN SURROUNDINGS ---
        for (size_t j = 0; j < vehicles.size(); j++) {
            if (i == j) continue;
            Vehicle* other = vehicles[j].get();
            if (other->finished) continue;

            float dist = GetDistance(current->position, other->position);
            if (dist > dynamicDetectionRange) continue;

            Vector3 toOther = Vector3Subtract(other->position, current->position);
            float fwdDist = Vector3DotProduct(toOther, current->forward);
            float sideDist = Vector3DotProduct(toOther, { -current->forward.z, 0, current->forward.x });

            float combinedHalfLengths = (current->length * 0.5f) + (other->length * 0.5f);

            // A. PHYSICAL BLOCK CHECK (Ignored if ANGRY Mode is ON)
            // We moved this logic to only affect 'emergencyStop' later
            
            float dot = Vector3DotProduct(current->forward, other->forward);

            if (dot > 0.7f) {
                // SAME DIRECTION
                if (IsInMyLane(current, other)) {
                    float physicalGap = dist - combinedHalfLengths;
                    if (physicalGap < closestGap) {
                        closestGap = physicalGap;
                        closestVehicle = other;
                        followMode = true;
                    }
                }
            } 
            else {
                // INTERSECTION
                float safeCrossingDist = combinedHalfLengths + 3.0f + (current->speed * 0.5f);
                if (fwdDist > 0 && fwdDist < safeCrossingDist && fabs(sideDist) < 2.5f) {
                    emergencyStop = true;
                }
            }
        }

        // --- ANGRY MODE (NUCLEAR OPTION) ---
        if (current->forceMoveTimer > 0.0f) {
            targetSpeed = 8.0f;     // Force high speed
            current->speed = 8.0f;  // Force physics velocity immediately
            emergencyStop = false;  // Ignore obstacles
            followMode = false;     // Ignore lead car
            
            // VISUAL: ANGRY MODE (RED)
            current->color = RED;
        }
        else {
            // VISUAL: CALM MODE (RESTORE ORIGINAL)
            current->color = current->originalColor;
        }

        // --- APPLY LOGIC (Only if NOT in Angry Mode) ---
        if (current->forceMoveTimer <= 0.0f) {
            if (emergencyStop) {
                targetSpeed = 0.0f;
                current->speed = 0.0f; 
            }
            else if (followMode) {
                if (closestGap < minSafeDist) {
                    targetSpeed = 0.0f;
                }
                else if (closestGap < dynamicSlowingDist) {
                     float factor = (closestGap - minSafeDist) / (dynamicSlowingDist - minSafeDist);
                     if (closestVehicle) {
                         targetSpeed = closestVehicle->speed + (current->desiredSpeed - closestVehicle->speed) * factor;
                     } else {
                         targetSpeed = current->desiredSpeed * factor;
                     }
                }
            }
            
            // Physics Smoothing
            float acceleration = 10.0f;
            float braking = 15.0f + (current->speed * 0.5f); 

            if (followMode && closestGap < minSafeDist + 2.0f && current->speed > 1.0f) {
                braking = 50.0f; 
            }

            if (current->speed > targetSpeed) {
                current->speed -= braking * dt;
                if (current->speed < targetSpeed) current->speed = targetSpeed;
            } 
            else {
                current->speed += acceleration * dt;
                if (current->speed > targetSpeed) current->speed = targetSpeed;
            }
        }
        
        if (current->speed < 0.0f) current->speed = 0.0f;
    }
}