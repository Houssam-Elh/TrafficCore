#include "basicmap.h"
#include <vector>
#include <cmath>
#include "draw_utils.h"

// ----- Constants -----
const float ROAD_WIDTH = 18.0f;
const float TWO_LANE_WIDTH = 10.0f;

// Main Roundabout Dimensions (Large)
const float ASPHALT_RADIUS = 26.0f;     
const float ISLAND_RADIUS = 14.0f; 

const float SIDEWALK_WIDTH = 4.0f;  
const float SIDEWALK_HEIGHT = 0.2f; 

// --- Returns {firstNodeID, lastNodeID} ---
std::pair<int, int> addArcPath(RoadGraph& graph, Vector3 center, float radius, float startAngle, float endAngle, int segments) {
    int startIdx = graph.GetAllNodes().size();
    float step = (endAngle - startAngle) / segments;

    for (int i = 0; i <= segments; i++) {
        float angle = (startAngle + i * step) * DEG2RAD;
        Vector3 pos = {
            center.x + cosf(angle) * radius,
            0.0f,
            center.z + sinf(angle) * radius
        };

        int id = graph.GetAllNodes().size();
        // 1. Initially set every node to ARC type
        graph.AddNode(id, pos, ARC);

        // Automatic internal chaining
        if (i > 0) {
            graph.ConnectNodes(id - 1, id);
        }
    }

    int endIdx = graph.GetAllNodes().size() - 1;
    
    // 2. OVERRIDE: Set the Entry and Exit as DECISION nodes
    graph.GetNode(startIdx).type = DECISION;
    graph.GetNode(endIdx).type = DECISION;

    return {startIdx, endIdx};
}

// --- BASIC MAP Drawings ---
void DrawBasicMap() {
    DrawPlane({0, -0.1f, 0}, {300, 300}, DARKGREEN);

    Color markColor = { 210, 210, 210, 255 };

    // --- MAIN ROADS ---
    DrawCube({0, -0.06f, 0}, ROAD_WIDTH, 0.0f, 240.0f, DARKGRAY); 
    DrawCube({0, -0.05f, 0}, 240.0f, 0.0f, ROAD_WIDTH, DARKGRAY); 

    // --- ROUNDABOUT SURFACE ---
    DrawCylinder({0, -0.04f, 0}, ASPHALT_RADIUS, ASPHALT_RADIUS, 0.2f, 40, DARKGRAY);
    
    //----- ROUNDED ARC ROADS ---
    DrawRoundedRoadArc({-39.0f, 0.0f, 39.0f}, ASPHALT_RADIUS + SIDEWALK_WIDTH, TWO_LANE_WIDTH, 0.0f, 0, 90, DARKGRAY);
    DrawRoundedRoadArc({39, 0.0f, 39}, ASPHALT_RADIUS + SIDEWALK_WIDTH, TWO_LANE_WIDTH, 0.0f, 90, 180, DARKGRAY);   
    DrawRoundedRoadArc({39, 0.0f, -39}, ASPHALT_RADIUS + SIDEWALK_WIDTH, TWO_LANE_WIDTH, 0.0f, 180, 270, DARKGRAY);   
    DrawRoundedRoadArc({-39, 0.0f, -39}, ASPHALT_RADIUS + SIDEWALK_WIDTH, TWO_LANE_WIDTH, 0.0f, 270, 360, DARKGRAY);   

    // --- CURVED SIDEWALK ARCS ---
    DrawSidewalkBlockArc({-39, 0.0f, 39}, ASPHALT_RADIUS, SIDEWALK_WIDTH, SIDEWALK_HEIGHT, 0, 90, LIGHTGRAY);   
    DrawSidewalkBlockArc({39, 0.0f, 39}, ASPHALT_RADIUS, SIDEWALK_WIDTH, SIDEWALK_HEIGHT, 90, 180, LIGHTGRAY); 
    DrawSidewalkBlockArc({39, 0.0f, -39}, ASPHALT_RADIUS, SIDEWALK_WIDTH, SIDEWALK_HEIGHT, 180, 270, LIGHTGRAY); 
    DrawSidewalkBlockArc({-39, 0.0f, -39}, ASPHALT_RADIUS, SIDEWALK_WIDTH, SIDEWALK_HEIGHT, 270, 360, LIGHTGRAY); 
        
                
    // --- CENTRAL ISLAND ---
    DrawCylinder({0, -0.03f, 0}, ISLAND_RADIUS, ISLAND_RADIUS, 0.3f, 32, GREEN);    
    DrawCylinderWires({0, -0.03f, 0}, ISLAND_RADIUS, ISLAND_RADIUS, 0.3f, 32, GRAY); 
    DrawCylinder({0, -0.02f, 0}, 1.0f, 1.0f, 2.0f, 8, BROWN);

    // --- STRAIGHT SIDEWALKS ---
    float roadExtent = 100.0f;
    // Inner sidewalk edge (11.0f) intersects R=26.0 at Z=23.6f
    float sidewalkStartBound = 39.0f; 
    float sideOffset = 11.0f; 

    // 1. East Side (Uninterrupted)
    float lenEast = roadExtent - sidewalkStartBound + 6.7f;
    float centerEast = sidewalkStartBound + (lenEast / 2.0f);
    DrawSidewalkSegment({ centerEast, 0.1f, -sideOffset}, lenEast, 4.0f);
    DrawSidewalkSegment({ centerEast, 0.1f,  sideOffset}, lenEast, 4.0f);

// -----------------------------------------------------------------------------------------------------------------

    // 2. North Side (INTERRUPTED by Road at Z = 85) - Split into 2 segments

    // Segment A: Roundabout (39.0) to 3RD SIDE ROAD (80.0)
    // The road at 85 has a width of 10 (TWO_LANE_WIDTH), so it cuts from 80 to 90.
    float zSA_startN = 39.0f; // approx -sidewalkStartBound
    float zSA_endN = 80.0f;   
    float lenSA_N = abs(zSA_endN - zSA_startN);
    float centerSA_N = (zSA_startN + zSA_endN) / 2.0f;

    DrawSidewalkSegment({-sideOffset, 0.1f, centerSA_N}, 4.0f, lenSA_N); 
    DrawSidewalkSegment({ sideOffset, 0.1f, centerSA_N}, 4.0f, lenSA_N); 

    // Segment B: 2ND SIDE ROAD (90.0) to Map Teleport End (120.0)
    float zSB_start_N = 90.0f;
    float zSB_end_N = 120.0f;
    float lenSB_N = abs(zSB_end_N - zSB_start_N);
    float centerSB_N = (zSB_start_N + zSB_end_N) / 2.0f;

    DrawSidewalkSegment({-sideOffset, 0.1f, centerSB_N}, 4.0f, lenSB_N); 
    DrawSidewalkSegment({ sideOffset, 0.1f, centerSB_N}, 4.0f, lenSB_N);

// -----------------------------------------------------------------------------------------------------------------

    // 3. South Side (INTERRUPTED by Road at Z = -85) - Split into 2 segments

    // Segment A: Roundabout (-39.0) to 2ND SIDE ROAD (-80.0)
    // The road at -85 has a width of 10 (TWO_LANE_WIDTH), so it cuts from -80 to -90.
    float zSA_start_S = -39.0f; // approx -sidewalkStartBound
    float zSA_end_S = -80.0f;   
    float lenSA_S = abs(zSA_end_S - zSA_start_S);
    float centerSA_S = (zSA_start_S + zSA_end_S) / 2.0f;

    DrawSidewalkSegment({-sideOffset, 0.1f, centerSA_S}, 4.0f, lenSA_S); 
    DrawSidewalkSegment({ sideOffset, 0.1f, centerSA_S}, 4.0f, lenSA_S);

    // Segment B: 2ND SIDE ROAD (-90.0) to Map Teleport End (-120.0)
    float zSB_start_S = -90.0f;
    float zSB_end_S = -120.0f;
    float lenSB_S = abs(zSB_end_S - zSB_start_S);
    float centerSB_S = (zSB_start_S + zSB_end_S) / 2.0f;

    DrawSidewalkSegment({-sideOffset, 0.1f, centerSB_S}, 4.0f, lenSB_S); 
    DrawSidewalkSegment({ sideOffset, 0.1f, centerSB_S}, 4.0f, lenSB_S);

// ---------------------------------------------------------------------------------------------------------------------------------------------

    // 4. West Side (INTERRUPTED by Road at X = -85) - Split into 2 segments
    // Segment A: Roundabout (-39.0) to Road Edge (-80)
    float xWA_start = -39.0f;
    float xWA_end = -80.0f; 
    float lenWA = abs(xWA_end - xWA_start);
    float centerWA = (xWA_start + xWA_end) / 2.0f;

    DrawSidewalkSegment({centerWA, 0.1f, -sideOffset}, lenWA, 4.0f);
    DrawSidewalkSegment({centerWA, 0.1f, sideOffset}, lenWA, 4.0f);

    // Segment B: Road Edge (-90) to End (-120)
    float xWB_start = -90.0f;
    float xWB_end = -120.0f;
    float lenWB = abs(xWB_end - xWB_start);
    float centerWB = (xWB_start + xWB_end) / 2.0f;

    DrawSidewalkSegment({centerWB, 0.1f, -sideOffset}, lenWB, 4.0f);
    DrawSidewalkSegment({centerWB, 0.1f, sideOffset}, lenWB, 4.0f);

// ---------------------------------------------------------------------------------------------------------------------------------------------

    // --- 1ST SIDE ROAD (X = -85) --- MIDDLE OF THE SCENE
    DrawCube({-85.0f, -0.055f, 0.0f}, TWO_LANE_WIDTH, 0.0f, 188.0f, DARKGRAY);
    
    // Dashed Lines
    for (float z = -85.0f; z < -12.0f; z += 4.0f) {
        DrawCube({-85.0f, -0.054f, z + 1.0f}, 0.3f, 0.05f, 2.0f, markColor);
    }
    for (float z = 12.0f; z < 85.0f; z += 4.0f) {
        DrawCube({-85.0f, -0.054f, z + 1.0f}, 0.3f, 0.05f, 2.0f, markColor);
    }

    // --- NEW ROAD SIDEWALKS (X = -85) ---
    float newRoadSidewalkOffset = 7.0f; 
    
    // North Part (Z < -9.0)
    float zN_start_Big = -94.0f;
    float zN_start_Small = -80.0f;
    float zN_end = -9.0f; 
    float lenN_Big = zN_end - zN_start_Big;
    float lenN_Small = zN_end - zN_start_Small;

    float centerN_Big = zN_start_Big + lenN_Big/2.0f;
    float centerN_Small = zN_start_Small + lenN_Small/2.0f;

    DrawSidewalkSegment({-85.0f - newRoadSidewalkOffset, 0.1f, centerN_Big}, 4.0f, lenN_Big);
    DrawSidewalkSegment({-85.0f + newRoadSidewalkOffset, 0.1f, centerN_Small}, 4.0f, lenN_Small);


    // South Part (Z > 9.0)
    float zS_start = 9.0f; 
    float zS_end_Big = 94.0f;
    float zS_end_Small = 80.0f;

    float lenS_Big = zS_end_Big - zS_start;
    float lenS_Small = zS_end_Small - zS_start;

    float centerS_Big = zS_start + lenS_Big/2.0f;
    float centerS_Small = zS_start + lenS_Small/2.0f;

    DrawSidewalkSegment({-85.0f - newRoadSidewalkOffset, 0.1f, centerS_Big}, 4.0f, lenS_Big);
    DrawSidewalkSegment({-85.0f + newRoadSidewalkOffset, 0.1f, centerS_Small}, 4.0f, lenS_Small);

    // --- CROSSWALKS (Moved to X = -85) ---
    DrawZebraCrossing({-85.0f, 0.0f, -13.0f}, TWO_LANE_WIDTH, 4.0f, 0.0f);
    DrawZebraCrossing({-85.0f, 0.0f, 13.0f}, TWO_LANE_WIDTH, 4.0f, 0.0f);
                
// ---------------------------------------------------------------------------------------------------------------------------------------------

    // --- 2ND SIDE ROAD (Z = -85) ---
    // Positioned at Z=-85.0f, running horizontally (parallel to X-axis)
    DrawCube({6.0f, -0.055f, -85.0f}, 188.0f, 0.0f, TWO_LANE_WIDTH, DARKGRAY); 
    
    // Dashed Lines (Horizontal road, vertical dashes)
    for (float x = -85.0f; x < -12.0f; x += 4.0f) {
        // Dash cube is 2.0f long (X) and 0.3f wide (Z)
        DrawCube({x + 1.0f, -0.054f, -85.0f}, 2.0f, 0.0f, 0.3f, markColor);
    }
    for (float x = 12.0f; x < 100.0f; x += 4.0f) {
        DrawCube({x + 1.0f, -0.054f, -85.0f}, 2.0f, 0.0f, 0.3f, markColor);
    }
    
    // East Part (X > 9.0) - Connects from the main road's boundary out to the end
    float xE_start = 9.0f;
    float xE_end = 100.0f; 
    float lenE = xE_end - xE_start;
    float centerE = xE_start + lenE/2.0f;

    DrawSidewalkSegment({centerE, 0.1f, -85.0f - newRoadSidewalkOffset}, lenE, 4.0f);
    DrawSidewalkSegment({centerE, 0.1f, -85.0f + newRoadSidewalkOffset}, lenE, 4.0f);

    // West Part (X < -9.0)
    float xW_start_Big = -94.0f;
    float xW_start_Small = -80.0f;
    float xW_end = -9.0f;

    float lenE_Big = xW_end - xW_start_Big;
    float lenE_Small = xW_end - xW_start_Small;

    float centerE_Big = xW_start_Big + lenE_Big/2.0f;
    float centerE_Small = xW_start_Small + lenE_Small/2.0f;

    DrawSidewalkSegment({centerE_Big, 0.1f, -85.0f - newRoadSidewalkOffset}, lenE_Big, 4.0f); // Big SideWalk
    DrawSidewalkSegment({centerE_Small, 0.1f, -85.0f + newRoadSidewalkOffset}, lenE_Small, 4.0f); // Big SideWalk

    // These are placed on the main road (X=0) crossing the new Z=-85 road.
    DrawZebraCrossing({-13.0f, 0.0f, -85.0f}, TWO_LANE_WIDTH, 4.0f, 90.0f); // West side of intersection
    DrawZebraCrossing({13.0f, 0.0f, -85.0f}, TWO_LANE_WIDTH, 4.0f, 90.0f); // East side of intersection

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // --- 3RD SIDE ROAD (Z = 85) ---
    // Positioned at Z=85.0f, running horizontally (parallel to X-axis)
    DrawCube({6.0f, -0.055f, 85.0f}, 188.0f, 0.0f, TWO_LANE_WIDTH, DARKGRAY); 
    
    // Dashed Lines (Horizontal road, vertical dashes)
    for (float x = -85.0f; x < -12.0f; x += 4.0f) {
        // Dash cube is 2.0f long (X) and 0.3f wide (Z)
        DrawCube({x + 1.0f, -0.054f, 85.0f}, 2.0f, 0.0f, 0.3f, markColor);
    }
    for (float x = 12.0f; x < 100.0f; x += 4.0f) {
        DrawCube({x + 1.0f, -0.054f, 85.0f}, 2.0f, 0.0f, 0.3f, markColor);
    }
    
    // East Part (X > 9.0) - Connects from the main road's boundary out to the end
    float xE_start_3rd = 9.0f;
    float xE_end_3rd = 100.0f; 
    float lenE_3rd = xE_end_3rd - xE_start_3rd;
    float centerE_3rd = xE_start_3rd + lenE_3rd/2.0f;

    DrawSidewalkSegment({centerE_3rd, 0.1f, 85.0f - newRoadSidewalkOffset}, lenE_3rd, 4.0f);
    DrawSidewalkSegment({centerE_3rd, 0.1f, 85.0f + newRoadSidewalkOffset}, lenE_3rd, 4.0f);

    // West Part (X < -9.0)
    float xW_start_Big_3rd = -94.0f;
    float xW_start_Small_3rd = -80.0f;
    float xW_end_3rd = -9.0f;

    float lenE_Big_3rd = xW_end_3rd - xW_start_Big_3rd;
    float lenE_Small_3rd = xW_end_3rd - xW_start_Small_3rd;

    float centerE_Big_3rd = xW_start_Big_3rd + lenE_Big_3rd/2.0f;
    float centerE_Small_3rd = xW_start_Small_3rd + lenE_Small_3rd/2.0f;

    DrawSidewalkSegment({centerE_Big_3rd, 0.1f, 85.0f + newRoadSidewalkOffset}, lenE_Big_3rd, 4.0f); // Big SideWalk
    DrawSidewalkSegment({centerE_Small_3rd, 0.1f, 85.0f - newRoadSidewalkOffset}, lenE_Small_3rd, 4.0f); // Small SideWalk

    // These are placed on the main road (X=0) crossing the new Z=85 road.
    DrawZebraCrossing({-13.0f, 0.0f, 85.0f}, TWO_LANE_WIDTH, 4.0f, 90.0f);
    DrawZebraCrossing({13.0f, 0.0f, 85.0f}, TWO_LANE_WIDTH, 4.0f, 90.0f);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // --- MARKINGS (Roundabout) ---
    DrawRingFlat({0, -0.035f, 0}, ISLAND_RADIUS + 0.2f, ISLAND_RADIUS + 0.5f, markColor);
    DrawRingFlat({0, -0.035f, 0}, ASPHALT_RADIUS - 0.5f, ASPHALT_RADIUS - 0.2f, markColor);

    float lineStart = ASPHALT_RADIUS + 0.5f; 
    float lineEnd = 120.0f;
    float lineLen = lineEnd - lineStart;
    float lineCenter = lineStart + (lineLen / 2.0f);

    DrawCube({-0.25f, 0.0f, -lineCenter}, 0.2f, 0.0f, lineLen, WHITE); 
    DrawCube({ 0.25f, 0.0f, -lineCenter}, 0.2f, 0.0f, lineLen, WHITE); 
    DrawCube({-0.25f, 0.0f,  lineCenter}, 0.2f, 0.0f, lineLen, WHITE); 
    DrawCube({ 0.25f, 0.0f,  lineCenter}, 0.2f, 0.0f, lineLen, WHITE); 
    DrawCube({-lineCenter, 0.0f, -0.25f}, lineLen, 0.0f, 0.2f, WHITE);
    DrawCube({ lineCenter, 0.0f, -0.25f}, lineLen, 0.0f, 0.2f, WHITE);
    DrawCube({-lineCenter, 0.0f,  0.25f}, lineLen, 0.0f, 0.2f, WHITE);
    DrawCube({ lineCenter, 0.0f,  0.25f}, lineLen, 0.0f, 0.2f, WHITE);

    for(int i=-120; i<120; i+=6) {
        if (abs(i) > ASPHALT_RADIUS) { 
            DrawCube({-4.5f, 0.0f, (float)i}, 0.2f, 0.0f, 2.0f, markColor);
            DrawCube({ 4.5f, 0.0f, (float)i}, 0.2f, 0.0f, 2.0f, markColor);
            DrawCube({(float)i, 0.0f, -4.5f}, 2.0f, 0.0f, 0.2f, markColor);
            DrawCube({(float)i, 0.0f,  4.5f}, 2.0f, 0.0f, 0.2f, markColor);
        }
    }

    // --- 4-WAY CROSSWALKS (Pushed back to 32.0f) ---
    DrawZebraCrossing({0.0f, 0.0f, 32.0f}, ROAD_WIDTH, 6.0f, 0.0f);
    DrawZebraCrossing({0.0f, 0.0f, -32.0f}, ROAD_WIDTH, 6.0f, 0.0f);
    DrawZebraCrossing({32.0f, 0.0f, 0.0f}, ROAD_WIDTH, 6.0f, 90.0f);
    DrawZebraCrossing({-32.0f, 0.0f, 0.0f}, ROAD_WIDTH, 6.0f, 90.0f);

    // --- TERMINAL ROUNDABOUT EXTENSION ---
    DrawTerminalRoundabout({120, 0, 0});
    DrawCube({110.0f, -0.05f, 0}, 20.0f, 0.0f, ROAD_WIDTH, DARKGRAY);

}

// --- Graph Building (Nodes & Connections) ---
void InitializeRoadNetwork(RoadGraph& graph) {

    // 1. Définition des Noeuds simples -------------------------------------------
    graph.AddNode( 0, { -120.0f,   0.0f,   6.75f },     START);
    graph.AddNode( 1, { -120.0f,   0.0f,   2.50f },     START);

    graph.AddNode( 2, {  -40.0f,   0.0f,   6.75f },  DECISION);  // Enter Roundabout
    graph.AddNode( 3, { -35.75f,   0.0f,   2.50f },  DECISION);

    graph.AddNode( 4, {  -6.75f,   0.0f,   40.0f },  DECISION);  // Exit Roundabout
    graph.AddNode( 5, {  -2.50f,   0.0f,  35.75f },  DECISION);

    graph.AddNode( 6, {  -6.75f,   0.0f,  120.0f },  TELEPORT);
    graph.AddNode( 7, {  -2.50f,   0.0f,  120.0f },  TELEPORT);

    graph.AddNode( 8, {   40.0f,   0.0f,  -6.75f },  DECISION);  // Enter Roundabout
    graph.AddNode( 9, {  35.75f,   0.0f,  -2.50f },  DECISION);

    graph.AddNode(10, {   6.75f,   0.0f,  -40.0f },  DECISION);  // Exit Roundabout
    graph.AddNode(11, {   2.50f,   0.0f, -35.75f },  DECISION);
    
    graph.AddNode(12, {  -6.75f,   0.0f,  -40.0f },  DECISION);  // Enter Roundabout
    graph.AddNode(13, {  -2.50f,   0.0f, -35.75f },  DECISION);

    graph.AddNode(14, {  -40.0f,   0.0f,  -6.75f },  DECISION);  // Exit Roundabout
    graph.AddNode(15, { -35.75f,   0.0f,  -2.50f },  DECISION);

    graph.AddNode(16, {   6.75f,   0.0f,   40.0f },  DECISION);  // Enter Roundabout
    graph.AddNode(17, {   2.50f,   0.0f,  35.75f },  DECISION);

    graph.AddNode(18, {   40.0f,   0.0f,   6.75f },  DECISION);  // Exit Roundabout
    graph.AddNode(19, {  35.75f,   0.0f,   2.50f },  DECISION);

    graph.AddNode(20, {  -87.5f,   0.0f,   6.75f },  DECISION);
    graph.AddNode(21, {  -82.5f,   0.0f,   6.75f },  DECISION);
    
    graph.AddNode(22, {  -87.5f,   0.0f,   87.5f },  DECISION);
    graph.AddNode(23, {  -82.5f,   0.0f,   82.5f },  DECISION);
    
    graph.AddNode(24, {  -6.75f,   0.0f,   87.5f },  DECISION);
    graph.AddNode(25, {  -6.75f,   0.0f,   82.5f },  DECISION);
    
    graph.AddNode(26, {   6.75f,   0.0f,   120.0f},     START);
    graph.AddNode(27, {   2.50f,   0.0f,   120.0f},     START);
    
    graph.AddNode(28, {   6.75f,   0.0f,  -120.0f},  TELEPORT);
    graph.AddNode(29, {   2.50f,   0.0f,  -120.0f},  TELEPORT);
    
    graph.AddNode(30, {  -6.75f,   0.0f,  -120.0f},     START);
    graph.AddNode(31, {  -2.50f,   0.0f,  -120.0f},     START);
    
    graph.AddNode(32, {   6.75f,   0.0f,   87.5f },  DECISION);
    graph.AddNode(33, {   6.75f,   0.0f,   82.5f },  DECISION);
    
    graph.AddNode(34, {  100.0f,   0.0f,   87.5f },  TELEPORT);
    graph.AddNode(35, {  100.0f,   0.0f,   82.5f },     START);
    
    graph.AddNode(36, {  106.5f,   0.0f,   6.75f },  DECISION);  // Enter Terminal Roundabout
    graph.AddNode(37, {  109.0f,   0.0f,   2.50f },  DECISION);
    
    graph.AddNode(38, {  106.5f,   0.0f,  -6.75f },  DECISION);  // Exit Terminal Roundabout
    graph.AddNode(39, {  109.0f,   0.0f,  -2.50f },  DECISION);
    
    graph.AddNode(40, {  -6.75f,   0.0f,   -87.5f},  DECISION);
    graph.AddNode(41, {  -6.75f,   0.0f,   -82.5f},  DECISION);
    
    graph.AddNode(42, {   6.75f,   0.0f,   -87.5f},  DECISION);
    graph.AddNode(43, {   6.75f,   0.0f,   -82.5f},  DECISION);
    
    graph.AddNode(44, { -120.0f,   0.0f,   -6.75f},  TELEPORT);
    graph.AddNode(45, { -120.0f,   0.0f,   -2.50f},  TELEPORT);
    
    graph.AddNode(46, {  -87.5f,   0.0f,  -6.75f },  DECISION);
    graph.AddNode(47, {  -82.5f,   0.0f,  -6.75f },  DECISION);
    
    graph.AddNode(48, {  -87.5f,   0.0f,  -87.5f },  DECISION);
    graph.AddNode(49, {  -82.5f,   0.0f,  -82.5f },  DECISION);
    
    graph.AddNode(50, {  100.0f,   0.0f,  -87.5f },     START);
    graph.AddNode(51, {  100.0f,   0.0f,  -82.5f },  TELEPORT);

    // 2. Création des ARCS -------------------------------------------------------------
    // --- Line 1 ---
    auto arc2_1   = addArcPath(graph, {     -39, 0.0f,     39}, 32.25f,  -90.0f, -45.0f,  10);
    auto arc2_2   = addArcPath(graph, {     -39, 0.0f,     39}, 32.25f,  -45.0f,   0.0f,  10);
    auto arc3_1   = addArcPath(graph, {  -34.25, 0.0f,  34.25}, 31.75f,  -90.0f, -45.0f,  10);
    auto arc3_2   = addArcPath(graph, {  -34.25, 0.0f,  34.25}, 31.75f,  -45.0f,   0.0f,  10);

    // --- Line 2 ---
    auto arc16_1  = addArcPath(graph, {      39, 0.0f,     39}, 32.25f, -180.0f, -135.0f, 10);
    auto arc16_2  = addArcPath(graph, {      39, 0.0f,     39}, 32.25f, -135.0f,  -90.0f, 10);
    auto arc17_1  = addArcPath(graph, {   34.25, 0.0f,  34.25}, 31.75f, -180.0f, -135.0f, 10);
    auto arc17_2  = addArcPath(graph, {   34.25, 0.0f,  34.25}, 31.75f, -135.0f,  -90.0f, 10);
    
    // --- Line 3 ---
    auto arc8_1   = addArcPath(graph, {      39, 0.0f,    -39}, 32.25f, -270.0f, -225.0f, 10);
    auto arc8_2   = addArcPath(graph, {      39, 0.0f,    -39}, 32.25f, -225.0f, -180.0f, 10);
    auto arc9_1   = addArcPath(graph, {   34.25, 0.0f, -34.25}, 31.75f, -270.0f, -225.0f, 10);
    auto arc9_2   = addArcPath(graph, {   34.25, 0.0f, -34.25}, 31.75f, -225.0f, -180.0f, 10);   

    // --- Line 4 ---
    auto arc12_1  = addArcPath(graph, {     -39, 0.0f,    -39}, 32.25f, -360.0f, -315.0f, 10);
    auto arc12_2  = addArcPath(graph, {     -39, 0.0f,    -39}, 32.25f, -315.0f, -270.0f, 10);
    auto arc13_1  = addArcPath(graph, {  -34.25, 0.0f, -34.25}, 31.75f, -360.0f, -315.0f, 10);
    auto arc13_2  = addArcPath(graph, {  -34.25, 0.0f, -34.25}, 31.75f, -315.0f, -270.0f, 10);
    
    // --- Main Roundabout ---
    auto arc_r1_1 = addArcPath(graph, {    0.0f, 0.0f,   0.0f}, 16.75f,  135.0f,   45.0f, 15); // Line 1
    auto arc_r1_2 = addArcPath(graph, {    0.0f, 0.0f,   0.0f}, 23.00f,  135.0f,   45.0f, 15);
    auto arc_r2_1 = addArcPath(graph, {    0.0f, 0.0f,   0.0f}, 16.75f,   45.0f,  -45.0f, 15); // Line 2
    auto arc_r2_2 = addArcPath(graph, {    0.0f, 0.0f,   0.0f}, 23.00f,   45.0f,  -45.0f, 15);
    auto arc_r3_1 = addArcPath(graph, {    0.0f, 0.0f,   0.0f}, 16.75f,  -45.0f, -135.0f, 15); // Line 3
    auto arc_r3_2 = addArcPath(graph, {    0.0f, 0.0f,   0.0f}, 23.00f,  -45.0f, -135.0f, 15);
    auto arc_r4_1 = addArcPath(graph, {    0.0f, 0.0f,   0.0f}, 16.75f, -135.0f, -225.0f, 15); // Line 4
    auto arc_r4_2 = addArcPath(graph, {    0.0f, 0.0f,   0.0f}, 23.00f, -135.0f, -225.0f, 15);

    // --- Terminal Roundabout ---
    auto arc_tr37 = addArcPath(graph, { 120.25f, 0.0f,   0.0f},  10.5f,  160.0f, -160.0f, 15);
    auto arc_tr36 = addArcPath(graph, { 120.50f, 0.0f,   0.0f},  14.0f,  150.0f, -150.0f, 15);


    // 3. CONNEXIONS (Utilisation de ConnectNodes) ----------------------------------------------------------------
    graph.ConnectNodes( 0, 20);
    graph.ConnectNodes(20, 22); graph.ConnectNodes(20,  2);
    graph.ConnectNodes(22, 24); graph.ConnectNodes(24,  6);
    graph.ConnectNodes( 4, 25);
    graph.ConnectNodes(25,  6); graph.ConnectNodes(25, 23);
    graph.ConnectNodes(23, 21); graph.ConnectNodes(21,  2);
    graph.ConnectNodes( 1,  3); graph.ConnectNodes( 5,  7);
    graph.ConnectNodes(27, 17); graph.ConnectNodes(26, 32);
    graph.ConnectNodes(32, 16); graph.ConnectNodes(32, 34);
    graph.ConnectNodes(35, 33); graph.ConnectNodes(33, 16);
    graph.ConnectNodes(18, 36); graph.ConnectNodes(19, 37);
    graph.ConnectNodes(39,  9); graph.ConnectNodes(38,  8);
    graph.ConnectNodes(11, 29); graph.ConnectNodes(10, 43);
    graph.ConnectNodes(43, 51); graph.ConnectNodes(43, 28);
    graph.ConnectNodes(50, 42); graph.ConnectNodes(42, 28);
    graph.ConnectNodes(31, 13); graph.ConnectNodes(30, 40);
    graph.ConnectNodes(40, 12); graph.ConnectNodes(40, 48);
    graph.ConnectNodes(48, 46); graph.ConnectNodes(46, 44);
    graph.ConnectNodes(15, 45); graph.ConnectNodes(14, 47);
    graph.ConnectNodes(47, 44); graph.ConnectNodes(47, 49);
    graph.ConnectNodes(49, 41); graph.ConnectNodes(41, 12);

    // 4. CONNEXIONS DES ARC -----------------------------------------------------------------------------
    // --- Line 1 ---
    graph.ConnectNodes(2, arc2_1.first);
    graph.ConnectNodes(arc2_1.second, arc2_2.first);
    graph.ConnectNodes(arc2_2.second, 4);
    graph.ConnectNodes(3, arc3_1.first);
    graph.ConnectNodes(arc3_1.second, arc3_2.first);
    graph.ConnectNodes(arc3_2.second, 5);

    // --- Line 2 ---
    graph.ConnectNodes(16, arc16_1.first);
    graph.ConnectNodes(arc16_1.second, arc16_2.first);
    graph.ConnectNodes(arc16_2.second, 18);
    graph.ConnectNodes(17, arc17_1.first);
    graph.ConnectNodes(arc17_1.second, arc17_2.first);
    graph.ConnectNodes(arc17_2.second, 19);

    // --- Line 3 ---
    graph.ConnectNodes(8, arc8_1.first);
    graph.ConnectNodes(arc8_1.second, arc8_2.first);
    graph.ConnectNodes(arc8_2.second, 10);
    graph.ConnectNodes(9, arc9_1.first);
    graph.ConnectNodes(arc9_1.second, arc9_2.first);
    graph.ConnectNodes(arc9_2.second, 11);

    // --- Line 4 ---
    graph.ConnectNodes(12, arc12_1.first);
    graph.ConnectNodes(arc12_1.second, arc12_2.first);
    graph.ConnectNodes(arc12_2.second, 14);
    graph.ConnectNodes(13, arc13_1.first);
    graph.ConnectNodes(arc13_1.second, arc13_2.first);
    graph.ConnectNodes(arc13_2.second, 15);

    // --- Main Roundabout ---
        // PART 1:
        graph.ConnectNodes(arc2_1.second, arc_r1_2.first);
        graph.ConnectNodes(arc_r1_2.second, arc16_2.first);
        graph.ConnectNodes(arc_r1_2.second, arc_r2_2.first);
        graph.ConnectNodes(arc3_1.second, arc_r1_1.first);
        graph.ConnectNodes(arc_r1_1.second, arc17_2.first);
        graph.ConnectNodes(arc_r1_1.second, arc_r2_1.first);
        // PART 2:
        graph.ConnectNodes(arc16_1.second, arc_r2_2.first);
        graph.ConnectNodes(arc_r2_2.second, arc8_2.first);
        graph.ConnectNodes(arc_r2_2.second, arc_r3_2.first);
        graph.ConnectNodes(arc17_1.second, arc_r2_1.first);
        graph.ConnectNodes(arc_r2_1.second, arc9_2.first);
        graph.ConnectNodes(arc_r2_1.second, arc_r3_1.first);
        // PART 3:
        graph.ConnectNodes(arc8_1.second, arc_r3_2.first);
        graph.ConnectNodes(arc_r3_2.second, arc12_2.first);
        graph.ConnectNodes(arc_r3_2.second, arc_r4_2.first);
        graph.ConnectNodes(arc9_1.second, arc_r3_1.first);
        graph.ConnectNodes(arc_r3_1.second, arc13_2.first);
        graph.ConnectNodes(arc_r3_1.second, arc_r4_1.first);
        // PART 4:
        graph.ConnectNodes(arc12_1.second, arc_r4_2.first);
        graph.ConnectNodes(arc_r4_2.second, arc2_2.first);
        graph.ConnectNodes(arc_r4_2.second, arc_r1_2.first);
        graph.ConnectNodes(arc13_1.second, arc_r4_1.first);
        graph.ConnectNodes(arc_r4_1.second, arc3_2.first);
        graph.ConnectNodes(arc_r4_1.second, arc_r1_1.first);
    
    // --- Terminal Roundabout ---
    graph.ConnectNodes(36, arc_tr36.first);
    graph.ConnectNodes(arc_tr36.second, 38);
    graph.ConnectNodes(37, arc_tr37.first);
    graph.ConnectNodes(arc_tr37.second, 39);
    
    // 5. TELEPORTS (Utilisation de SetTeleportTarget)
    graph.SetTeleportTarget( 6, 30);
    graph.SetTeleportTarget( 7, 31);
    graph.SetTeleportTarget(29, 26);
    graph.SetTeleportTarget(28, 27);
    graph.SetTeleportTarget(44, 50);
    graph.SetTeleportTarget(45, 35);
    graph.SetTeleportTarget(34,  0);
    graph.SetTeleportTarget(51,  1);

}
