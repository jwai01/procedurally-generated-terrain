#include "PerlinNoise.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

PerlinNoise::PerlinNoise() {
    // Initialize the permutation array with values 0-255
    for (int i = 0; i < 256; i++) {
        p[i] = i;
    }
    
    // Shuffle the permutation array
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 255; i > 0; i--) {
        int j = rand() % (i + 1);
        std::swap(p[i], p[j]);
    }
    
    // Duplicate the permutation array
    for (int i = 0; i < 256; i++) {
        p[i + 256] = p[i];
    }
}

PerlinNoise::~PerlinNoise() {}


float PerlinNoise::fade(float t) const {
    // Fade function: 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::lerp(float t, float a, float b) const {
    // Linear interpolation
    return a + t * (b - a);
}

float PerlinNoise::grad(int hash, float x, float y, float z) const {
    // Convert hash to 8 gradient directions
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float PerlinNoise::noise(float x, float y, float z) const {
    // Find unit cube that contains the point
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    int Z = static_cast<int>(std::floor(z)) & 255;
    
    // Find relative x, y, z of point in cube
    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);
    
    // Compute fade curves
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);
    
    // Hash coordinates of the 8 cube corners
    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;
    
    // Add blended results from 8 corners of cube
    return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
                                   grad(p[BA], x-1, y, z)),
                           lerp(u, grad(p[AB], x, y-1, z),
                                   grad(p[BB], x-1, y-1, z))),
                   lerp(v, lerp(u, grad(p[AA+1], x, y, z-1),
                                   grad(p[BA+1], x-1, y, z-1)),
                           lerp(u, grad(p[AB+1], x, y-1, z-1),
                                   grad(p[BB+1], x-1, y-1, z-1))));
}

float PerlinNoise::noise(float x, float y) const {
    // Call the 3D noise function with z=0
    return noise(x, y, 0.0f);
}