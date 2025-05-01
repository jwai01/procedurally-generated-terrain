#include <iostream>
#include "terrain/TerrainGenerator.h"
#include "renderer/Renderer.h"

int main() {
    std::cout << "Procedural Terrain Generator" << std::endl;
    
    // Configuration
    int width = 256;
    int height = 256;
    float scale = 50.0f;
    int octaves = 4;
    float persistence = 0.5f;
    float lacunarity = 2.0f;
    
    // Generate terrain
    TerrainGenerator terrainGenerator;
    HeightMap heightMap = terrainGenerator.generateTerrain(
        width, height, scale, octaves, persistence, lacunarity
    );
    
    // Create and configure renderer
    Renderer renderer;
    if (!renderer.initialize(width, height, "Procedural Terrain")) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    // Render loop
    while (!renderer.shouldClose()) {
        renderer.renderTerrain(heightMap);
        renderer.update();  // This now handles input and timing
    }
    
    renderer.cleanup();
    return 0;
}