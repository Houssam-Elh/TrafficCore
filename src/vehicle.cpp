#include "vehicle.h"
#include "rlgl.h"
#include "raymath.h" // Important pour Vector3Normalize, etc.

// =============================================================================
//  VEHICLE BASE CLASS
// =============================================================================

Vehicle::Vehicle(Vector3 pos, int initialTargetId) 
    : position(pos), 
      forward({1,0,0}), 
      speed(5.0f), 
      desiredSpeed(5.0f), 
      length(4.0f), 
      targetNodeId(initialTargetId), 
      color(RED),
      originalColor(RED),
      forceMoveTimer(0.0f)
      {}

Vehicle::~Vehicle() {}

// MISE À JOUR : Utilise RoadGraph au lieu de std::vector<Node>
void Vehicle::update(float dt, RoadGraph &graph, const std::vector<std::unique_ptr<Vehicle>>& allVehicles) {

    // 1. Récupération sécurisée du noeud cible via la classe RoadGraph
    Node &targetNode = graph.GetNode(targetNodeId);
    Vector3 targetPos = targetNode.pos;
    
    // 2. Calcul de la direction et distance
    Vector3 dir = { targetPos.x - position.x, targetPos.y - position.y, targetPos.z - position.z };
    float dist = sqrtf(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
    
    // 3. LOGIQUE D'ARRIVÉE
    if (dist < CONFIG::ARRIVAL_THRESHOLD) { // Threshold for "reaching" the node
        
        // TYPE A: TELEPORTATION
        if (targetNode.type == TELEPORT) {
            int startNodeId = targetNode.teleportTargetId;
            Node &destinationNode = graph.GetNode(startNodeId);

            // --- 1. CHECK IF LANDING ZONE IS CLEAR ---
            bool isBlocked = false;

            
            
            // Scan all vehicles to see if anyone is at the destination
            for (const auto& other : allVehicles) {
                // Don't check against self
                if (other.get() == this) continue; 

                // Check distance to the destination node
                // 8.0f is a safe gap to ensure we don't spawn inside them
                if (Vector3Distance(other->position, destinationNode.pos) < 8.0f) {
                    isBlocked = true;
                    break;
                }
            }

            // --- 2. EXECUTE TELEPORT OR WAIT ---
            if (!isBlocked) {
                // CLEAR: Jump instantly
                this->position = destinationNode.pos;
                this->targetNodeId = destinationNode.nextNodes[0];
                
                // IMPORTANT: Reset direction immediately to face the new path
                Node &nextNode = graph.GetNode(this->targetNodeId);
                Vector3 newDir = Vector3Subtract(nextNode.pos, this->position);
                this->forward = Vector3Normalize(newDir);
            } 
            else {
                // BLOCKED: Stop and wait for the car ahead to move
                this->speed = 0; 
                return; 
            }
        }
        
        // TYPE B: NAVIGATION CLASSIQUE (DECISION, START, ARC)
        else if (targetNode.type == DECISION || targetNode.type == START || targetNode.type == ARC) {
            if (!targetNode.nextNodes.empty()) {
                // Pick one of multiple paths randomly
                int randomIndex = GetRandomValue(0, targetNode.nextNodes.size() - 1);
                targetNodeId = targetNode.nextNodes[randomIndex];
            }
        }
        return; // Exit update for this frame to prevent jitter
    }

    // 4. LOGIQUE DE MOUVEMENT
        // A- Normalize direction vector (using the dist we calculated earlier)
        if (dist > 0) {
            dir.x /= dist; 
            dir.y /= dist; 
            dir.z /= dist;
        }

        // B- Smooth Steering (Interpolate forward vector toward direction)
        float turnRate = 12.0f * dt;
        forward.x += (dir.x - forward.x) * turnRate;
        forward.z += (dir.z - forward.z) * turnRate;

        // C- Re-normalize forward vector to keep speed consistent
        float fMag = sqrtf(forward.x*forward.x + forward.z*forward.z);
        if (fMag > 0) { 
            forward.x /= fMag; 
            forward.z /= fMag; 
        }

        // D- Apply velocity to position
        position.x += forward.x * speed * dt;
        position.y += forward.y * speed * dt;
        position.z += forward.z * speed * dt;
}

void Vehicle::draw() {
    float angle = atan2f(forward.x, forward.z) * RAD2DEG;
    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(angle, 0, 1, 0);
    DrawCube({0,0,0}, 2.0f, 0.6f, 4.0f, color); 
    DrawCubeWires({0,0,0}, 2.0f, 0.6f, 4.0f, BLACK);
    rlPopMatrix();
}

void DrawWheel3D(float x, float y, float z, float radius = 0.3f, float width = 0.4f) {
    rlPushMatrix();
        rlTranslatef(x, y, z);
        // Rotate 90 deg around Z so the cylinder lays flat sideways
        rlRotatef(90, 0, 0, 1); 
        // Note: Raylib draws cylinder centered at (0,0,0). 
        // The rotation pivots it correctly.
        DrawCylinder((Vector3){0,0,0}, radius, radius, width, 16, BLACK);
        DrawCylinderWires((Vector3){0,0,0}, radius, radius, width, 16, DARKGRAY);
        
        // Hubcap (Visual detail to see rotation)
        DrawCylinder((Vector3){0, width/2.0f + 0.01f, 0}, radius*0.5f, radius*0.5f, 0.05f, 8, LIGHTGRAY);
    rlPopMatrix();
}

// =============================================================================
//  CAR IMPLEMENTATION
// =============================================================================

Car::Car(Vector3 pos, int targetId) : Vehicle(pos, targetId) { 
    color = BLUE; 
    originalColor = BLUE;
    desiredSpeed = CONFIG::CAR_SPEED; 
    speed = desiredSpeed;
    length = 4.5f; // Standard Car Length
}

void Car::draw() {
    float angle = atan2f(forward.x, forward.z) * RAD2DEG;
    
    rlPushMatrix();
        // Move to car position and rotate to face direction
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(angle, 0, 1, 0); 

        // 1. Chassis
        DrawCube((Vector3){0, 0.4f, 0}, 2.0f, 0.5f, 4.0f, color);
        DrawCubeWires((Vector3){0, 0.4f, 0}, 2.0f, 0.5f, 4.0f, DARKBLUE);

        // 2. Cabin
        DrawCube((Vector3){0, 0.9f, -0.3f}, 1.6f, 0.5f, 2.0f, SKYBLUE); 
        DrawCubeWires((Vector3){0, 0.9f, -0.3f}, 1.6f, 0.5f, 2.0f, BLACK);

        // 3. Wheels (Car Width is 2.0, so wheels at +/- 1.1 is perfect)
        DrawWheel3D(-1.1f, 0.3f, 1.2f);  // Front Left
        DrawWheel3D( 1.1f, 0.3f, 1.2f);  // Front Right
        DrawWheel3D(-1.1f, 0.3f, -1.2f); // Rear Left
        DrawWheel3D( 1.1f, 0.3f, -1.2f); // Rear Right

        // Lights
        DrawCube((Vector3){-0.6f, 0.4f, 2.0f}, 0.3f, 0.2f, 0.1f, YELLOW);
        DrawCube((Vector3){ 0.6f, 0.4f, 2.0f}, 0.3f, 0.2f, 0.1f, YELLOW);
        DrawCube((Vector3){-0.6f, 0.4f, -2.0f}, 0.3f, 0.2f, 0.1f, RED);
        DrawCube((Vector3){ 0.6f, 0.4f, -2.0f}, 0.3f, 0.2f, 0.1f, RED);

    rlPopMatrix();
}

// =============================================================================
//  BUS IMPLEMENTATION
// =============================================================================

Bus::Bus(Vector3 pos, int targetId) : Vehicle(pos, targetId) { 
    color = GOLD;
    originalColor = GOLD;
    desiredSpeed = CONFIG::BUS_SPEED;
    speed = desiredSpeed; 
    length = 8.5f;
}

void Bus::draw() {
    float angle = atan2f(forward.x, forward.z) * RAD2DEG;
    
    rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(angle, 0, 1, 0);

        // 1. Body
        DrawCube((Vector3){0, 0.9f, 0}, 2.2f, 1.5f, 7.0f, color);
        DrawCubeWires((Vector3){0, 0.9f, 0}, 2.2f, 1.5f, 7.0f, DARKBROWN);

        // Windows
        DrawCube((Vector3){1.11f, 1.2f, 0}, 0.1f, 0.6f, 6.0f, DARKGRAY); 
        DrawCube((Vector3){-1.11f, 1.2f, 0}, 0.1f, 0.6f, 6.0f, DARKGRAY); 
        DrawCube((Vector3){0, 1.2f, 3.51f}, 2.0f, 0.6f, 0.1f, DARKGRAY); 

        // 3. Wheels
        DrawWheel3D(-1.25f, 0.4f, 2.2f, 0.4f);  
        DrawWheel3D( 1.25f, 0.4f, 2.2f, 0.4f);  
        DrawWheel3D(-1.25f, 0.4f, -2.2f, 0.4f); 
        DrawWheel3D( 1.25f, 0.4f, -2.2f, 0.4f);

    rlPopMatrix();
}

// =============================================================================
//  TRUCK IMPLEMENTATION
// =============================================================================

Truck::Truck(Vector3 pos, int targetId) : Vehicle(pos, targetId) { 
    color = (Color){139, 69, 19, 255}; // Brun
    originalColor = (Color){139, 69, 19, 255};
    desiredSpeed = CONFIG::TRUCK_SPEED;
    speed = desiredSpeed;
    length = 10.0f; // Truck is the longest
}

void Truck::draw() {
    float angle = atan2f(forward.x, forward.z) * RAD2DEG;
    
    rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(angle, 0, 1, 0);

        // Cab
        DrawCube((Vector3){0, 0.7f, 2.5f}, 2.0f, 1.2f, 2.5f, color);
        DrawCubeWires((Vector3){0, 0.7f, 2.5f}, 2.0f, 1.2f, 2.5f, BLACK);
        DrawCube((Vector3){0, 1.1f, 3.5f}, 1.8f, 0.6f, 0.1f, SKYBLUE); 
        
        // Trailer
        DrawCube((Vector3){0, 1.0f, -2.0f}, 2.4f, 1.8f, 6.0f, GRAY);
        DrawCubeWires((Vector3){0, 1.0f, -2.0f}, 2.4f, 1.8f, 6.0f, DARKGRAY);

        // Wheels
        DrawWheel3D(-1.35f, 0.4f, 2.8f, 0.4f);  // Front (Cab)
        DrawWheel3D( 1.35f, 0.4f, 2.8f, 0.4f); 
        
        DrawWheel3D(-1.35f, 0.4f, -1.5f, 0.45f); // Mid (Trailer)
        DrawWheel3D( 1.35f, 0.4f, -1.5f, 0.45f); 
        
        DrawWheel3D(-1.35f, 0.4f, -3.5f, 0.45f); // Rear (Trailer)
        DrawWheel3D( 1.35f, 0.4f, -3.5f, 0.45f); 

        // Stop lights
        DrawCube((Vector3){-1.0f, 1.0f, -5.0f}, 0.4f, 0.3f, 0.1f, RED);
        DrawCube((Vector3){ 1.0f, 1.0f, -5.0f}, 0.4f, 0.3f, 0.1f, RED);

    rlPopMatrix();
}

// =============================================================================
//  TAXI IMPLEMENTATION
// =============================================================================

Taxi::Taxi(Vector3 pos, int targetId) : Vehicle(pos, targetId) { 
    color = YELLOW;
    originalColor = YELLOW;
    desiredSpeed = CONFIG::TAXI_SPEED;
    speed = desiredSpeed;
    length = 4.5f;
}

void Taxi::update(float dt, RoadGraph &graph, const std::vector<std::unique_ptr<Vehicle>> &allVehicles) {
    Vehicle::update(dt, graph, allVehicles); 
    signBlinkTimer += dt;
}

void Taxi::draw() {
    float angle = atan2f(forward.x, forward.z) * RAD2DEG;
    
    rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(angle, 0, 1, 0);

        // Body
        DrawCube((Vector3){0, 0.4f, 0}, 2.0f, 0.5f, 4.0f, color);
        DrawCubeWires((Vector3){0, 0.4f, 0}, 2.0f, 0.5f, 4.0f, GOLD);
        DrawCube((Vector3){0, 0.9f, -0.3f}, 1.6f, 0.5f, 2.0f, SKYBLUE); 
        DrawCubeWires((Vector3){0, 0.9f, -0.3f}, 1.6f, 0.5f, 2.0f, BLACK);

        // Taxi Sign
        bool lightOn = (int)(signBlinkTimer * 2.0f) % 2 == 0;
        Color signColor = lightOn ? (Color){255, 100, 0, 255} : (Color){100, 50, 0, 255};
        DrawCube((Vector3){0, 1.4f, -0.3f}, 1.0f, 0.2f, 0.5f, signColor);
        DrawCubeWires((Vector3){0, 1.4f, -0.3f}, 1.0f, 0.2f, 0.5f, BLACK);

        // Wheels
        DrawWheel3D(-1.1f, 0.3f, 1.2f);
        DrawWheel3D( 1.1f, 0.3f, 1.2f);
        DrawWheel3D(-1.1f, 0.3f, -1.2f);
        DrawWheel3D( 1.1f, 0.3f, -1.2f);

        // Lights
        DrawCube((Vector3){-0.6f, 0.4f, 2.0f}, 0.3f, 0.2f, 0.1f, YELLOW);
        DrawCube((Vector3){ 0.6f, 0.4f, 2.0f}, 0.3f, 0.2f, 0.1f, YELLOW);
        DrawCube((Vector3){-0.6f, 0.4f, -2.0f}, 0.3f, 0.2f, 0.1f, RED);
        DrawCube((Vector3){ 0.6f, 0.4f, -2.0f}, 0.3f, 0.2f, 0.1f, RED);

    rlPopMatrix();
}

// =============================================================================
//  POLICE CAR IMPLEMENTATION
// =============================================================================

PoliceCar::PoliceCar(Vector3 pos, int targetId) : Vehicle(pos, targetId) { 
    color = (Color){20, 20, 120, 255}; // Bleu foncé
    originalColor = (Color){20, 20, 120, 255};
    desiredSpeed = CONFIG::POLICE_SPEED;
    speed = desiredSpeed;
    length = 4.5f;
}

void PoliceCar::update(float dt, RoadGraph &graph, const std::vector<std::unique_ptr<Vehicle>> &allVehicles) {
    Vehicle::update(dt, graph, allVehicles); 
    sirenTimer += dt;
}

void PoliceCar::draw() {
    float angle = atan2f(forward.x, forward.z) * RAD2DEG;
    
    rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(angle, 0, 1, 0);

        // Body
        DrawCube((Vector3){0, 0.4f, 0}, 2.0f, 0.5f, 4.0f, color);
        DrawCubeWires((Vector3){0, 0.4f, 0}, 2.0f, 0.5f, 4.0f, WHITE);
        DrawCube((Vector3){1.05f, 0.4f, 0}, 0.1f, 0.5f, 3.0f, WHITE); 
        DrawCube((Vector3){-1.05f, 0.4f, 0}, 0.1f, 0.5f, 3.0f, WHITE); 

        DrawCube((Vector3){0, 0.9f, -0.3f}, 1.6f, 0.5f, 2.0f, (Color){40, 40, 60, 255});
        DrawCubeWires((Vector3){0, 0.9f, -0.3f}, 1.6f, 0.5f, 2.0f, BLACK);

        // Siren
        float sirenPhase = fmodf(sirenTimer * 3.0f, 2.0f);
        Color sirenColor = sirenPhase < 1.0f ? RED : BLUE;
        DrawCylinder((Vector3){0, 1.3f, -0.3f}, 0.3f, 0.3f, 0.3f, 8, sirenColor);
        DrawCylinderWires((Vector3){0, 1.3f, -0.3f}, 0.3f, 0.3f, 0.3f, 8, DARKGRAY);

        // Wheels
        DrawWheel3D(-1.1f, 0.3f, 1.2f);
        DrawWheel3D( 1.1f, 0.3f, 1.2f);
        DrawWheel3D(-1.1f, 0.3f, -1.2f);
        DrawWheel3D( 1.1f, 0.3f, -1.2f);

        // Lights
        DrawCube((Vector3){-0.6f, 0.4f, 2.0f}, 0.3f, 0.2f, 0.1f, WHITE);
        DrawCube((Vector3){ 0.6f, 0.4f, 2.0f}, 0.3f, 0.2f, 0.1f, WHITE);
        DrawCube((Vector3){-0.6f, 0.4f, -2.0f}, 0.3f, 0.2f, 0.1f, RED);
        DrawCube((Vector3){ 0.6f, 0.4f, -2.0f}, 0.3f, 0.2f, 0.1f, RED);

    rlPopMatrix();
}

// =============================================================================
//  MOTORCYCLE IMPLEMENTATION
// =============================================================================

Motorcycle::Motorcycle(Vector3 pos, int targetId) : Vehicle(pos, targetId) { 
    color = (Color){50, 50, 50, 255}; // Gris foncé
    originalColor = (Color){50, 50, 50, 255};
    desiredSpeed = CONFIG::MOTORCYCLE_SPEED;
    speed = desiredSpeed;
    length = 2.5f; // Shortest vehicle
    lastForward = forward;
}

void Motorcycle::update(float dt, RoadGraph &graph, const std::vector<std::unique_ptr<Vehicle>> &allVehicles) {
    Vector3 prevForward = lastForward;
    
    // Utilise votre logique de navigation RoadGraph
    Vehicle::update(dt, graph, allVehicles);

    // Calcul de l'inclinaison basé sur le changement de direction
    float turnRate = (forward.x - prevForward.x) + (forward.z - prevForward.z);
    tiltAngle = turnRate * 30.0f;
    
    // Limitation de l'angle d'inclinaison
    if (tiltAngle > 25.0f) tiltAngle = 25.0f;
    if (tiltAngle < -25.0f) tiltAngle = -25.0f;
    
    tiltAngle *= 0.9f; // Amortissement pour plus de fluidité
    lastForward = forward;
}

void Motorcycle::draw() {
    float angle = atan2f(forward.x, forward.z) * RAD2DEG;
    
    rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(angle, 0, 1, 0);
        rlRotatef(tiltAngle, 0, 0, 1); // Lean during turns

        // 1. Body and seat
        DrawCube((Vector3){0, 0.5f, 0}, 0.3f, 0.3f, 2.5f, color);
        DrawCube((Vector3){0, 0.7f, 0.2f}, 0.5f, 0.3f, 0.8f, RED); // Tank
        DrawCubeWires((Vector3){0, 0.7f, 0.2f}, 0.5f, 0.3f, 0.8f, DARKGRAY);
        DrawCube((Vector3){0, 0.9f, 0.8f}, 1.0f, 0.1f, 0.1f, DARKGRAY); // Handlebars
        DrawCube((Vector3){0, 0.8f, -0.5f}, 0.5f, 0.2f, 0.8f, BLACK); // Seat

        // 2. Wheels (Front and Rear)
        auto DrawBikeWheel = [](float z) {
            rlPushMatrix();
                rlTranslatef(0, 0.3f, z);
                rlRotatef(90, 0, 0, 1);
                DrawCylinder((Vector3){0,0,0}, 0.3f, 0.3f, 0.15f, 16, BLACK);
                DrawCylinderWires((Vector3){0,0,0}, 0.3f, 0.3f, 0.15f, 16, GRAY);
            rlPopMatrix();
        };
        DrawBikeWheel(1.0f);
        DrawBikeWheel(-1.2f);

        // 3. Lights (Front yellow, Rear red)
        DrawCube((Vector3){0, 0.6f, 1.3f}, 0.3f, 0.2f, 0.1f, YELLOW);
        DrawCube((Vector3){0, 0.6f, -1.3f}, 0.2f, 0.15f, 0.1f, RED);

    rlPopMatrix();
}