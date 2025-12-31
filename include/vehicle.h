#ifndef VEHICLE_H
#define VEHICLE_H

#include "raylib.h"
#include <vector>
#include <cmath>
#include <memory>

#include "config.h"    // Pour CONFIG::TRUCK_SPEED, etc.
#include "roadgraph.h" // Pour la classe RoadGraph et la structure Node

// ----- Classes de Base -----
class Vehicle {
public:
    Vector3 position;
    Vector3 forward;
    float speed;
    float desiredSpeed;
    float length;
    int targetNodeId;
    Color color;
    Color originalColor;
    bool finished = false;
    float forceMoveTimer = 0.0f;

    // Constructeur
    Vehicle(Vector3 pos, int initialTargetId);

    virtual ~Vehicle();

    // MISE À JOUR : Utilise RoadGraph au lieu de std::vector<Node>
    virtual void update(float dt, RoadGraph &graph, const std::vector<std::unique_ptr<Vehicle>>& allVehicles);

    virtual void draw();
};

class Car : public Vehicle {
public:
    // Pass pos and targetId to the base Vehicle constructor
    Car(Vector3 pos, int targetId);

    // Override the draw function to render a detailed car
    void draw() override;
};

class Bus : public Vehicle {
public:
    // Pass pos and targetId to the base Vehicle constructor
    Bus(Vector3 pos, int targetId);

    void draw() override;
};

class Truck : public Vehicle {
public:
    Truck(Vector3 pos, int targetId);

    void draw() override;
};

class Taxi : public Vehicle {
private:
    float signBlinkTimer = 0.0f;
    
public:
    // Constructeur adapté à votre système (position + cible)
    Taxi(Vector3 pos, int targetId);

    // Mise à jour de la logique spécifique (clignotant du toit)
    void update(float dt, RoadGraph &graph, const std::vector<std::unique_ptr<Vehicle>> &allVehicles) override;

    void draw() override;
};

class PoliceCar : public Vehicle {
private:
    float sirenTimer = 0.0f;
    
public:
    // Constructeur adapté : prend la position de départ et l'ID du premier noeud cible
    PoliceCar(Vector3 pos, int targetId);

    // Mise à jour de la sirène et de la navigation
    void update(float dt, RoadGraph &graph, const std::vector<std::unique_ptr<Vehicle>> &allVehicles) override;

    void draw() override;
};

class Motorcycle : public Vehicle {
private:
    float tiltAngle = 0.0f;
    Vector3 lastForward = {1, 0, 0};
    
public:
    // Constructeur adapté : position de départ et ID du premier nœud cible
    Motorcycle(Vector3 pos, int targetId);

    // Mise à jour de l'inclinaison et de la navigation
    void update(float dt, RoadGraph &graph, const std::vector<std::unique_ptr<Vehicle>> &allVehicles) override;

    void draw() override;
};

#endif // VEHICLE_H