#pragma once

#include "HeightMap.h"

class TerrainGenerator {
public:
    TerrainGenerator();
    ~TerrainGenerator();
    
    HeightMap generateTerrain(
        int width, 
        int height, 
        float scale, 
        int octaves, 
        float persistence, 
        float lacunarity
    );
    
private:
    float* generateNoiseMap(
        int width, 
        int height, 
        float scale, 
        int octaves, 
        float persistence, 
        float lacunarity
    );
};