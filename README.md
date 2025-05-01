# Procedural Terrain Generation

A C++ application for generating realistic 3D terrain using Perlin noise. This project visualizes procedurally generated landscapes with proper coloring based on elevation levels - from deep water to snowy mountains.

## Features
- Height map generation using Perlin noise
- Realistic terrain rendering with elevation-based coloring:
  - Deep and shallow water (blues)
  - Beaches/sand (tan)
  - Grasslands/forests (green)
  - Rocky terrain/mountains (gray/brown)
  - Snow-capped peaks (white)
- Interactive 3D camera system
- Configurable terrain parameters
- Smooth terrain transitions

## Dependencies
- GLFW and OpenGL for rendering
- GLEW for OpenGL extension loading
- GLM for mathematics
- C++17 compatible compiler

### Installing Dependencies
On Ubuntu/Debian-based systems, you can use the provided script:
```bash
./dependencies.sh
```

For manual installation:
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake git
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev mesa-common-dev
sudo apt-get install libglfw3 libglfw3-dev
sudo apt-get install libglew-dev
sudo apt-get install libglm-dev
```

## Building and Running
The easiest way to build and run the project is with the provided script:

```bash
# Make executable
chmod +x runthis.sh

# Build and run
./runthis.sh
```

### Manual Building
If you prefer to build manually:

```bash
mkdir build
cd build
cmake ..
cmake --build .
./TerrainGenerator
```

## Controls
- **W/A/S/D** - Change look direction (up/left/down/right)
- **O** - Move forward
- **L** - Move backward


## Project Structure
- `src/noise/` - Perlin noise implementation
- `src/terrain/` - Terrain generation algorithms
- `src/renderer/` - OpenGL rendering code
- `src/camera/` - Camera system for navigation

