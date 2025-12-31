#include "roadgraph.h"
#include "config.h" // Pour utiliser les couleurs centralisées

RoadGraph::RoadGraph() {}
RoadGraph::~RoadGraph() {}

void RoadGraph::AddNode(int id, Vector3 pos, NodeType type) {
    Node newNode(id, pos, type);
    newNode.nextAvailableTime = 0.0f; // Initialisation explicite
    nodes.push_back(newNode);
}

void RoadGraph::ConnectNodes(int fromId, int toId) {
    // On cherche le nœud source par son ID pour ajouter la connexion
    for (auto& node : nodes) {
        if (node.id == fromId) {
            node.nextNodes.push_back(toId);
            break;
        }
    }
}

Node& RoadGraph::GetNode(int id) {
    // Recherche sécurisée de l'ID
    for (auto& node : nodes) {
        if (node.id == id) return node;
    }
    return nodes[0]; // Sécurité par défaut
}

const std::vector<Node>& RoadGraph::GetAllNodes() const {
    return nodes;
}

void RoadGraph::SetTeleportTarget(int nodeId, int targetId) {
    for (auto& node : nodes) {
        if (node.id == nodeId) {
            node.teleportTargetId = targetId;
            break;
        }
    }
}

void RoadGraph::Clear() {
    nodes.clear();
}

void RoadGraph::DrawNodes() {
    
    for (const auto& n : nodes) {
        // --- DESSIN DES SPHÈRES ---
        // ONLY draw the sphere if it is NOT an ARC node
        if (n.type != ARC) {
            Color nodeColor = (n.type == START) ? GREEN : (n.type == TELEPORT ? RED : YELLOW);
            DrawSphere(n.pos, 1.0f, nodeColor);
        }

        // --- DESSIN DES LIGNES DE CONNEXION ---
        for (int nextId : n.nextNodes) {
            
            Vector3 nextPos = GetNode(nextId).pos;

            // Draw a line from the current node to its destination
            DrawLine3D(
                { n.pos.x, n.pos.y + 0.5f, n.pos.z }, 
                { nextPos.x, nextPos.y + 0.5f, nextPos.z }, 
                YELLOW
            );
        }
    }
}

void RoadGraph::DrawIdNodes(Camera3D camera) {
     
    // Cette partie doit techniquement être appelée quand on est en mode 2D, 
    // mais Raylib permet GetWorldToScreen pour projeter les IDs.
    
    for (const auto& n : nodes) {
        if (n.type != ARC) {

            // Convert 3D position to 2D screen position
            Vector2 screenPos = GetWorldToScreen({n.pos.x, n.pos.y + 2.5f, n.pos.z}, camera);
            
            // Only draw if the node is actually in front of the camera
            if (screenPos.x > 0 && screenPos.y > 0) {
                DrawText(TextFormat("ID:%d", n.id), screenPos.x - 10, screenPos.y, 10, BLACK);
            }
        }
    }
}