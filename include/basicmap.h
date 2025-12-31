#ifndef BASICMAP_H
#define BASICMAP_H

#include "roadgraph.h"
#include "raylib.h"

// Gère le dessin de la partie visuelle (Basic Map)
void DrawBasicMap();

// Gère l'initialisation de tous les nœuds et arcs (Logique)
void InitializeRoadNetwork(RoadGraph& graph);

#endif