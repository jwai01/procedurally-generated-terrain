#pragma once

class PerlinNoise {
public:
    PerlinNoise();
    ~PerlinNoise();
    
    float noise(float x, float y) const;
    float noise(float x, float y, float z) const;
    
    float fractalNoise(float x, float y, int octaves, float persistence, float lacunarity, float scale) const;
    
private:
    int p[512];
    
    float fade(float t) const;
    float lerp(float t, float a, float b) const;
    float grad(int hash, float x, float y, float z) const;
};