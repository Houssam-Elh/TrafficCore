#ifndef DRAW_UTILS_H
#define DRAW_UTILS_H

#include "raylib.h"
#include "rlgl.h" 
#include <vector>
#include <cmath>

void DrawArcSegment(Vector3 center, float innerRadius, float width, float startAngle, float endAngle, Color color);
void DrawSidewalkBlockArc(Vector3 center, float innerRadius, float width, float height, float startAngle, float endAngle, Color color);
void DrawRoundedRoadArc(Vector3 center, float innerRadius, float width, float height, float startAngle, float endAngle, Color color);
void DrawRoadLineArc(Vector3 center, float innerRadius, float width, float height, float startAngle, float endAngle, Color color);
void DrawRingFlat(Vector3 center, float innerRadius, float outerRadius, Color color);
void DrawZebraCrossing(Vector3 center, float roadWidth, float stripeLength, float angle);
void DrawTerminalRoundabout(Vector3 center);
void DrawSidewalkSegment(Vector3 pos, float lenX, float lenZ);

#endif