#pragma once

class HeightMap {
public:
    HeightMap(int width, int height, float* data);
    HeightMap(const HeightMap& other);
    HeightMap& operator=(const HeightMap& other);
    ~HeightMap();
    
    float getHeight(int x, int y) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    const float* getData() const { return data; }
    
private:
    int width;
    int height;
    float* data;
};