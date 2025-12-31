#include "draw_utils.h"


const float SIDEWALK_WIDTH = 4.0f;  
const float SIDEWALK_HEIGHT = 0.2f;

// ----- 2D Helper (Flat for Markings) -----
void DrawArcSegment(Vector3 center, float innerRadius, float width, float startAngle, float endAngle, Color color) {
    rlBegin(RL_QUADS);
    rlColor4ub(color.r, color.g, color.b, color.a);
    
    int segments = 32;
    float step = (endAngle - startAngle) / segments;
    float outerRadius = innerRadius + width;

    for (int i = 0; i < segments; i++) {
        float angle1 = (startAngle + i * step) * DEG2RAD;
        float angle2 = (startAngle + (i + 1) * step) * DEG2RAD;
        
        float x1_in = center.x + cosf(angle1) * innerRadius;
        float z1_in = center.z + sinf(angle1) * innerRadius;
        float x1_out = center.x + cosf(angle1) * outerRadius;
        float z1_out = center.z + sinf(angle1) * outerRadius;
        
        float x2_in = center.x + cosf(angle2) * innerRadius;
        float z2_in = center.z + sinf(angle2) * innerRadius;
        float x2_out = center.x + cosf(angle2) * outerRadius;
        float z2_out = center.z + sinf(angle2) * outerRadius;
        
        rlVertex3f(x1_in, center.y, z1_in);
        rlVertex3f(x1_out, center.y, z1_out);
        rlVertex3f(x2_out, center.y, z2_out);
        rlVertex3f(x2_in, center.y, z2_in);
    }
    rlEnd();
}

// ----- Draw Sidewalk Block Arc -----
void DrawSidewalkBlockArc(Vector3 center, float innerRadius, float width, float height, float startAngle, float endAngle, Color color) {
    float angleStep = 4.0f; 
    
    float radiusToCenterOfBlock = innerRadius + (width / 2.0f);
    float segmentLength = radiusToCenterOfBlock * (angleStep * DEG2RAD) * 1.05f;

    for (float a = startAngle; a < endAngle; a += angleStep) {
        float currentAngle = a + (angleStep / 2.0f); 
        
        rlPushMatrix();
            rlTranslatef(center.x, center.y + height/2.0f, center.z);
            rlRotatef(currentAngle, 0, 1, 0);
            rlTranslatef(radiusToCenterOfBlock, 0, 0);
            
            DrawCube({0,0,0}, width, height, segmentLength, color);
            DrawCubeWires({0,0,0}, width, height, segmentLength, GRAY);
        rlPopMatrix();
    }
}

// ----- Draw Rounded Road Arc --------
void DrawRoundedRoadArc(Vector3 center, float innerRadius, float width, float height, float startAngle, float endAngle, Color color) {
    float angleStep = 4.0f; 
    
    float radiusToCenterOfBlock = innerRadius + (width / 2.0f);
    float segmentLength = radiusToCenterOfBlock * (angleStep * DEG2RAD) * 1.05f;

    for (float a = startAngle; a < endAngle; a += angleStep) {
        float currentAngle = a + (angleStep / 2.0f); 
        
        rlPushMatrix();
            rlTranslatef(center.x, center.y + height/2.0f, center.z);
            rlRotatef(currentAngle, 0, 1, 0);
            rlTranslatef(radiusToCenterOfBlock, 0, 0);
            
            DrawCube({0,0,0}, width, height, segmentLength, color);
            DrawCubeWires({0,0,0}, width, height, segmentLength, DARKGRAY);
        rlPopMatrix();
    }
}

// ----- Draw Road Line Arc --------
void DrawRoadLineArc(Vector3 center, float innerRadius, float width, float height, float startAngle, float endAngle, Color color) {
    float angleStep = 4.0f; 
    
    float radiusToCenterOfBlock = innerRadius + (width / 2.0f);
    float segmentLength = radiusToCenterOfBlock * (angleStep * DEG2RAD) * 1.05f;
    
    
    for (float a = startAngle; a < endAngle; a += angleStep) {
        float currentAngle = a + (angleStep / 2.0f);

        rlPushMatrix();
            rlTranslatef(center.x, center.y + height/2.0f, center.z);
            rlRotatef(currentAngle, 0, 1, 0);
            rlTranslatef(radiusToCenterOfBlock, 0, 0);
            
            DrawCube({0,0,0}, width, height, segmentLength, color);
            DrawCubeWires({0,0,0}, width, height, segmentLength, WHITE);
        rlPopMatrix();
    }
}

// Reuse Ring helper for lines
void DrawRingFlat(Vector3 center, float innerRadius, float outerRadius, Color color) {
    DrawArcSegment(center, innerRadius, outerRadius - innerRadius, 0, 360, color);
}

// ----- Draw Zebra Crossing -----
void DrawZebraCrossing(Vector3 center, float roadWidth, float stripeLength, float angle) {
    int numStripes = 8; 
    float totalWidth = roadWidth - 2.0f; 
    float stripeWidth = totalWidth / (numStripes * 2.0f - 1.0f); 
    
    float startX = -(totalWidth / 2.0f) + (stripeWidth / 2.0f);
    
    Color stripeColor = { 210, 210, 210, 255 };

    rlPushMatrix();
        rlTranslatef(center.x, center.y, center.z);
        rlRotatef(angle, 0, 1, 0); 

        for (int i = 0; i < numStripes; i++) {
            float xPos = startX + i * (stripeWidth * 2.0f);
            DrawCube({xPos, -0.035f, 0.0f}, stripeWidth, 0.1f, stripeLength, stripeColor);
        }
    rlPopMatrix();
}

// ----- Helper: Terminal Roundabout (UPDATED to 16.0f) -----
void DrawTerminalRoundabout(Vector3 center) {
    Color markColor = { 210, 210, 210, 255 }; 
    
    // Reduced radius as requested
    float termRadius = 16.0f; 
    float termIsland = 8.0f;

    DrawCylinder({center.x, -0.04f, center.z}, termRadius, termRadius, 0.2f, 40, DARKGRAY);
    DrawCylinder({center.x, -0.03f, center.z}, termIsland, termIsland, 0.3f, 32, GREEN);
    DrawCylinderWires({center.x, -0.03f, center.z}, termIsland, termIsland, 0.3f, 32, GRAY);
    DrawCylinder({center.x, -0.02f, center.z}, 1.0f, 1.0f, 2.0f, 8, BROWN);

    DrawRingFlat({center.x, -0.035f, center.z}, termIsland + 0.2f, termIsland + 0.5f, markColor);
    DrawRingFlat({center.x, -0.035f, center.z}, termRadius - 0.5f, termRadius - 0.2f, markColor);

    DrawSidewalkBlockArc({center.x, 0.0f, center.z}, termRadius, SIDEWALK_WIDTH, SIDEWALK_HEIGHT, 214.6, 361.6, LIGHTGRAY);
    DrawSidewalkBlockArc({center.x, 0.0f, center.z}, termRadius, SIDEWALK_WIDTH, SIDEWALK_HEIGHT, 0-2.4, 144.6, LIGHTGRAY);
}

// ----- Draw Sidewalk Segment -----
void DrawSidewalkSegment(Vector3 pos, float lenX, float lenZ) {
    DrawCube(pos, lenX, SIDEWALK_HEIGHT, lenZ, LIGHTGRAY);
    DrawCubeWires(pos, lenX, SIDEWALK_HEIGHT, lenZ, GRAY);
}
