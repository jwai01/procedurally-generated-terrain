#include "HeightMap.h"

HeightMap::HeightMap(int width, int height, float* data) 
    : width(width), height(height) {
    this->data = new float[width * height];
    for (int i = 0; i < width * height; i++) {
        this->data[i] = data[i];
    }
}

HeightMap::HeightMap(const HeightMap& other) 
    : width(other.width), height(other.height) {
    data = new float[width * height];
    for (int i = 0; i < width * height; i++) {
        data[i] = other.data[i];
    }
}

HeightMap& HeightMap::operator=(const HeightMap& other) {
    if (this != &other) {
        delete[] data;
        width = other.width;
        height = other.height;
        data = new float[width * height];
        for (int i = 0; i < width * height; i++) {
            data[i] = other.data[i];
        }
    }
    return *this;
}

HeightMap::~HeightMap() {
    delete[] data;
}

float HeightMap::getHeight(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 0.0f;
    }
    return data[y * width + x];
}