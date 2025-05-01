#include "TerrainGenerator.h"
#include "../noise/PerlinNoise.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

TerrainGenerator::TerrainGenerator() {
    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

TerrainGenerator::~TerrainGenerator() {}

HeightMap TerrainGenerator::generateTerrain(
    int width, int height, float scale, int octaves, float persistence, float lacunarity
) {
    float* noiseMap = generateNoiseMap(width, height, scale, octaves, persistence, lacunarity);
    HeightMap heightMap(width, height, noiseMap);
    return heightMap;
}

float* TerrainGenerator::generateNoiseMap(
    int width, int height, float scale, int octaves, float persistence, float lacunarity
) {
    PerlinNoise noise;
    float* noiseMap = new float[width * height];
    
    // Random offsets for each octave
    float* octaveOffsets = new float[octaves * 2];
    for (int i = 0; i < octaves; i++) {
        float offsetX = static_cast<float>(rand() % 100000) - 50000.0f;
        float offsetY = static_cast<float>(rand() % 100000) - 50000.0f;
        octaveOffsets[i * 2] = offsetX;
        octaveOffsets[i * 2 + 1] = offsetY;
    }
    
    // Ensure scale is valid
    if (scale <= 0) {
        scale = 0.0001f;
    }
    
    float maxNoiseHeight = 0.0f;
    float minNoiseHeight = 1.0f;
    
    // Generate noise map
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float noiseHeight = 0.0f;
            
            // Sum octaves
            for (int i = 0; i < octaves; i++) {
                float sampleX = x / scale * frequency + octaveOffsets[i * 2];
                float sampleY = y / scale * frequency + octaveOffsets[i * 2 + 1];
                
                int octaves = 10;          // Number of layers (adjust for more/less detail) More octaves add more detail but take longer to compute
                float persistence = 0.9f; // (0 - 1)) Higher values (closer to 1) make details more prominent, Lower values make the terrain smoother with less detailed features
                float lacunarity = 2.0f;  // How quickly frequency increases (typically 2) Higher values add more small details
                float scale = 450.0f;     // Base terrain scale (higher = smoother) Smaller values create more jagged terrain with smaller features

                // Replace the single noise call with fractal noise
                float height = noise.fractalNoise(sampleX, sampleY, octaves, persistence, lacunarity, scale);
                noiseHeight += height * amplitude;
                
                amplitude *= persistence;
                frequency *= lacunarity;
            }
            
            // Update min and max values
            maxNoiseHeight = std::max(maxNoiseHeight, noiseHeight);
            minNoiseHeight = std::min(minNoiseHeight, noiseHeight);
            
            noiseMap[y * width + x] = noiseHeight;
        }
    }
    
    // Normalize noise map
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float normalizedHeight = (noiseMap[y * width + x] - minNoiseHeight) / (maxNoiseHeight - minNoiseHeight);
            noiseMap[y * width + x] = normalizedHeight;
        }
    }
    
    delete[] octaveOffsets;
    return noiseMap;
}