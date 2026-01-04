#ifndef ROADGRAPH_H
#define ROADGRAPH_H

#include "raylib.h"
#include <vector>

// Inclusion de votre logique de types
enum NodeType { START, TELEPORT, DECISION, ARC };

struct Node {
    int id;
    Vector3 pos;
    NodeType type;
    std::vector<int> nextNodes;
    int teleportTargetId;
    float nextAvailableTime = 0.0f; // Temps avant le prochain spawn autorisé

    Node(int id = 0, Vector3 p = {0,0,0}, NodeType t = DECISION) 
        : id(id), pos(p), type(t), teleportTargetId(-1) {}
};

class RoadGraph {
private:
    std::vector<Node> nodes; // Conteneur interne des noeuds

public:
    RoadGraph();
    ~RoadGraph();

    // Allow cleaning the graph
    void Clear();

    // Méthodes de gestion (Mélange de votre logique et celle du collègue)
    void AddNode(int id, Vector3 pos, NodeType type);
    void ConnectNodes(int fromId, int toId);
    Node& GetNode(int id); // Accès sécurisé au noeud
    const std::vector<Node>& GetAllNodes() const;
    
    // Pour votre logique de téléportation
    void SetTeleportTarget(int nodeId, int targetId);

    // DESSIN DES SPHÈRES ET DES LIGNES (Dans le monde 3D)
    void DrawNodes();

    // DESSIN DES TEXTES (IDs)
    void DrawIdNodes(Camera3D camera);
};

#endif