# TrafficCore Simulator (v2.2)

**TrafficCore** is a 3D urban traffic simulation engine built in C++ using the [Raylib](https://www.raylib.com/) library. It features a dynamic road network, intelligent vehicle AI, and a fully interactive user interface for configuring simulation parameters in real-time.

## 🚦 Features

* **Diverse Vehicle Types**: Includes Cars, Buses, Trucks, Taxis (with roof lights), Police Cars (with sirens), and Motorcycles (with tilt mechanics).
* **Intelligent Traffic AI**:
    * Collision avoidance system.
    * Lane detection and safe distance maintenance.
    * Speed regulation based on traffic conditions.
* **Dynamic Road Network**:
    * Graph-based road system with nodes and connections.
    * Support for intersections, decision points, and curved paths.
* **Interactive UI**:
    * **Main Menu**: animated background with start/exit options.
    * **Settings Menu**: Configure max vehicle count, spawn rates per vehicle type, and simulation speed.
    * **In-Game HUD**: Real-time stats and controls.
* **3D Camera System**: Free-roaming camera to inspect the simulation from any angle.

## 🛠️ Tech Stack

* **Language**: C++ (C++14 standard)
* **Graphics Library**: [Raylib 5.0.0](https://www.raylib.com/)
* **Build System**: GNU Make

## 📋 Prerequisites

Before building the project, ensure you have the following installed:

1.  **C++ Compiler**: `g++` (MinGW for Windows, GCC for Linux/macOS).
2.  **Make**: GNU Make tool.
3.  **Raylib**: The project requires the Raylib library.
    * *Default Path (Windows)*: `C:/raylib/raylib`
    * *Default Path (Linux)*: Standard system include paths or update `Makefile`.

## 🚀 How to Build & Run

### 1. Clone or Extract
Ensure you are in the project root directory:
```bash
cd TrafficCore_2.2v