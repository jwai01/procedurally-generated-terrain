#pragma once

#include <string>
#include <vector>  // Add this include for std::vector
#include "../terrain/HeightMap.h"
#include "../camera/Camera.h"  // Add camera include

// Forward declarations for GLFW types
struct GLFWwindow;

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool initialize(int width, int height, const std::string& title);
    bool shouldClose();
    void renderTerrain(const HeightMap& heightMap);
    void update();
    void handleInput(float deltaTime);  
    void cleanup();
    
private:
    GLFWwindow* window;
    int width;
    int height;
    
    void setupTerrainMesh(const HeightMap& heightMap);
    void renderMesh();
    
    // OpenGL resource IDs
    unsigned int vao;
    unsigned int vbo;
    unsigned int ibo;
    unsigned int shaderProgram;
    unsigned int totalIndicesCount; // Add this to track total indices
    
    // Shader helper methods
    unsigned int compileShader(const char* source, unsigned int type);
    unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
    
    // Add camera
    Camera camera;
    
    // Timing for smooth movement
    float lastFrame;
    float deltaTime;
    
    // Input handling
    void processInput();

    
    glm::vec3 getTerrainColor(float height) const;
    
    // Height processing
    float flattenWaterAreas(float height) const;
    
    // Tree generation
    void addTreeAt(std::vector<float>& vertices, std::vector<unsigned int>& indices, 
                 float x, float y, float z, float scale, int& vertexCount);
};