#include "Renderer.h"
#include <iostream>
#include <vector>
#include <cmath>  // Add this at the top with your other includes
#include "../camera/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// If we're on Windows
#ifdef _WIN32
#include <Windows.h>
#endif

// Include GLFW and OpenGL headers
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

// Basic shader sources
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    
    out vec3 vertexColor;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        vertexColor = aColor;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 vertexColor;
    out vec4 FragColor;
    
    void main() {
        FragColor = vec4(vertexColor, 1.0);
    }
)";

Renderer::Renderer() 
    : window(nullptr), vao(0), vbo(0), ibo(0), shaderProgram(0),
      camera(glm::vec3(0.0f, 10.0f, 5.0f)), // x, z, y postion of camera inital
      lastFrame(0.0f),
      deltaTime(0.0f),
      totalIndicesCount(0) {} // Initialize the new member variable

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize(int width, int height, const std::string& title) {
    this->width = width;
    this->height = height;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    window = glfwCreateWindow(800, 600, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW (on non-Apple platforms)
#ifndef __APPLE__
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return false;
    }
#endif
    
    // Create and compile shaders
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0) {
        std::cerr << "Failed to create shader program" << std::endl;
        return false;
    }
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    return true;
}

bool Renderer::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Renderer::renderTerrain(const HeightMap& heightMap) {
    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up terrain mesh if needed
    if (vao == 0) {
        setupTerrainMesh(heightMap);
    }
    
    // Render the mesh
    renderMesh();
}

void Renderer::update() {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    // Handle input
    handleInput(deltaTime);
    
    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::cleanup() {
    // Delete OpenGL resources
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    
    if (ibo != 0) {
        glDeleteBuffers(1, &ibo);
        ibo = 0;
    }
    
    if (shaderProgram != 0) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
    
    // Clean up GLFW
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    
    glfwTerminate();
}

// Get terrain color based on height
glm::vec3 Renderer::getTerrainColor(float height) const {
    // Define terrain thresholds
    const float waterLevel = 0.0f;
    const float sandLevel = 0.3f;
    const float grassLevel = 0.35f;
    const float rockLevel = 0.4f;
    const float snowLevel = 0.7f;

    // Map height to color
    if (height < waterLevel) {
        // Deep water - dark blue
        return glm::vec3(0.0f, 0.0f, 0.5f);
    } else if (height < sandLevel) {
        // Shallow water - lighter blue
        float t = (height - waterLevel) / (sandLevel - waterLevel);
        return glm::vec3(0.0f, 0.3f * t, 0.7f);
    } else if (height < grassLevel) {
        // Sand/beach - yellow/tan
        float t = (height - sandLevel) / (grassLevel - sandLevel);
        return glm::vec3(0.76f, 0.7f, 0.5f);
    } else if (height < rockLevel) {
        // Grass/forest - green
        float t = (height - grassLevel) / (rockLevel - grassLevel);
        return glm::mix(glm::vec3(0.1f, 0.6f, 0.1f), glm::vec3(0.1f, 0.4f, 0.1f), t);
    } else if (height < snowLevel) {
        // Rock/mountain - gray/brown
        float t = (height - rockLevel) / (snowLevel - rockLevel);
        return glm::mix(glm::vec3(0.5f, 0.4f, 0.3f), glm::vec3(0.5f, 0.5f, 0.5f), t);
    } else {
        // Snow - white
        float t = std::min((height - snowLevel) * 2.0f, 1.0f);
        return glm::mix(glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f), t);
    }
}

// Add triangular trees to vertices and indices arrays at specified position
void Renderer::addTreeAt(std::vector<float>& vertices, std::vector<unsigned int>& indices, 
                        float x, float y, float z, float scale, int& vertexCount) {
    // Tree colors - dark to light green
    glm::vec3 darkGreen(0.0f, 0.4f, 0.0f);
    glm::vec3 midGreen(0.1f, 0.5f, 0.1f);
    glm::vec3 lightGreen(0.2f, 0.6f, 0.1f);
    
    float treeHeight = 0.8f * scale;
    float baseWidth = 0.2f * scale;
    
    // Tree trunk (brown)
    float trunkHeight = 0.2f * scale;
    glm::vec3 brown(0.45f, 0.30f, 0.15f);
    
    // Add trunk vertices
    // Base of trunk
    vertices.push_back(x - 0.05f * scale);  // x
    vertices.push_back(y);                  // y
    vertices.push_back(z - 0.05f * scale);  // z
    vertices.push_back(brown.r);            // r
    vertices.push_back(brown.g);            // g
    vertices.push_back(brown.b);            // b
    
    vertices.push_back(x + 0.05f * scale);  // x
    vertices.push_back(y);                  // y
    vertices.push_back(z - 0.05f * scale);  // z
    vertices.push_back(brown.r);            // r
    vertices.push_back(brown.g);            // g
    vertices.push_back(brown.b);            // b
    
    vertices.push_back(x + 0.05f * scale);  // x
    vertices.push_back(y);                  // y
    vertices.push_back(z + 0.05f * scale);  // z
    vertices.push_back(brown.r);            // r
    vertices.push_back(brown.g);            // g
    vertices.push_back(brown.b);            // b
    
    vertices.push_back(x - 0.05f * scale);  // x
    vertices.push_back(y);                  // y
    vertices.push_back(z + 0.05f * scale);  // z
    vertices.push_back(brown.r);            // r
    vertices.push_back(brown.g);            // g
    vertices.push_back(brown.b);            // b
    
    // Top of trunk
    vertices.push_back(x - 0.05f * scale);  // x
    vertices.push_back(y + trunkHeight);    // y
    vertices.push_back(z - 0.05f * scale);  // z
    vertices.push_back(brown.r);            // r
    vertices.push_back(brown.g);            // g
    vertices.push_back(brown.b);            // b
    
    vertices.push_back(x + 0.05f * scale);  // x
    vertices.push_back(y + trunkHeight);    // y
    vertices.push_back(z - 0.05f * scale);  // z
    vertices.push_back(brown.r);            // r
    vertices.push_back(brown.g);            // g
    vertices.push_back(brown.b);            // b
    
    vertices.push_back(x + 0.05f * scale);  // x
    vertices.push_back(y + trunkHeight);    // y
    vertices.push_back(z + 0.05f * scale);  // z
    vertices.push_back(brown.r);            // r
    vertices.push_back(brown.g);            // g
    vertices.push_back(brown.b);            // b
    
    vertices.push_back(x - 0.05f * scale);  // x
    vertices.push_back(y + trunkHeight);    // y
    vertices.push_back(z + 0.05f * scale);  // z
    vertices.push_back(brown.r);            // r
    vertices.push_back(brown.g);            // g
    vertices.push_back(brown.b);            // b
    
    // Trunk indices
    unsigned int trunkBase = vertexCount;
    
    // Front face
    indices.push_back(trunkBase);
    indices.push_back(trunkBase + 1);
    indices.push_back(trunkBase + 5);
    
    indices.push_back(trunkBase);
    indices.push_back(trunkBase + 5);
    indices.push_back(trunkBase + 4);
    
    // Right face
    indices.push_back(trunkBase + 1);
    indices.push_back(trunkBase + 2);
    indices.push_back(trunkBase + 6);
    
    indices.push_back(trunkBase + 1);
    indices.push_back(trunkBase + 6);
    indices.push_back(trunkBase + 5);
    
    // Back face
    indices.push_back(trunkBase + 2);
    indices.push_back(trunkBase + 3);
    indices.push_back(trunkBase + 7);
    
    indices.push_back(trunkBase + 2);
    indices.push_back(trunkBase + 7);
    indices.push_back(trunkBase + 6);
    
    // Left face
    indices.push_back(trunkBase + 3);
    indices.push_back(trunkBase);
    indices.push_back(trunkBase + 4);
    
    indices.push_back(trunkBase + 3);
    indices.push_back(trunkBase + 4);
    indices.push_back(trunkBase + 7);
    
    vertexCount += 8;
    
    // Tree foliage (green triangular pyramids stacked)
    // First layer (bottom)
    float baseY = y + trunkHeight;
    
    // Bottom pyramid apex
    vertices.push_back(x);                  // x
    vertices.push_back(baseY + treeHeight * 0.6f); // y
    vertices.push_back(z);                  // z
    vertices.push_back(darkGreen.r);        // r
    vertices.push_back(darkGreen.g);        // g
    vertices.push_back(darkGreen.b);        // b
    
    // Bottom pyramid base vertices
    vertices.push_back(x - baseWidth);      // x
    vertices.push_back(baseY);              // y
    vertices.push_back(z - baseWidth);      // z
    vertices.push_back(darkGreen.r);        // r
    vertices.push_back(darkGreen.g);        // g
    vertices.push_back(darkGreen.b);        // b
    
    vertices.push_back(x + baseWidth);      // x
    vertices.push_back(baseY);              // y
    vertices.push_back(z - baseWidth);      // z
    vertices.push_back(darkGreen.r);        // r
    vertices.push_back(darkGreen.g);        // g
    vertices.push_back(darkGreen.b);        // b
    
    vertices.push_back(x + baseWidth);      // x
    vertices.push_back(baseY);              // y
    vertices.push_back(z + baseWidth);      // z
    vertices.push_back(darkGreen.r);        // r
    vertices.push_back(darkGreen.g);        // g
    vertices.push_back(darkGreen.b);        // b
    
    vertices.push_back(x - baseWidth);      // x
    vertices.push_back(baseY);              // y
    vertices.push_back(z + baseWidth);      // z
    vertices.push_back(darkGreen.r);        // r
    vertices.push_back(darkGreen.g);        // g
    vertices.push_back(darkGreen.b);        // b
    
    // Add lower pyramid triangles
    unsigned int lowerPyramidBase = vertexCount;
    unsigned int lowerPyramidApex = lowerPyramidBase;
    unsigned int lowerPyramidBottomLeft = lowerPyramidBase + 1;
    unsigned int lowerPyramidBottomRight = lowerPyramidBase + 2;
    unsigned int lowerPyramidTopRight = lowerPyramidBase + 3;
    unsigned int lowerPyramidTopLeft = lowerPyramidBase + 4;
    
    // Four faces of the pyramid
    indices.push_back(lowerPyramidApex);
    indices.push_back(lowerPyramidBottomLeft);
    indices.push_back(lowerPyramidBottomRight);
    
    indices.push_back(lowerPyramidApex);
    indices.push_back(lowerPyramidBottomRight);
    indices.push_back(lowerPyramidTopRight);
    
    indices.push_back(lowerPyramidApex);
    indices.push_back(lowerPyramidTopRight);
    indices.push_back(lowerPyramidTopLeft);
    
    indices.push_back(lowerPyramidApex);
    indices.push_back(lowerPyramidTopLeft);
    indices.push_back(lowerPyramidBottomLeft);
    
    vertexCount += 5;
    
    // Second layer (middle)
    float midY = baseY + treeHeight * 0.4f;
    float midWidth = baseWidth * 0.7f;
    
    // Middle pyramid apex
    vertices.push_back(x);                  // x
    vertices.push_back(midY + treeHeight * 0.4f); // y
    vertices.push_back(z);                  // z
    vertices.push_back(midGreen.r);         // r
    vertices.push_back(midGreen.g);         // g
    vertices.push_back(midGreen.b);         // b
    
    // Middle pyramid base vertices
    vertices.push_back(x - midWidth);       // x
    vertices.push_back(midY);               // y
    vertices.push_back(z - midWidth);       // z
    vertices.push_back(midGreen.r);         // r
    vertices.push_back(midGreen.g);         // g
    vertices.push_back(midGreen.b);         // b
    
    vertices.push_back(x + midWidth);       // x
    vertices.push_back(midY);               // y
    vertices.push_back(z - midWidth);       // z
    vertices.push_back(midGreen.r);         // r
    vertices.push_back(midGreen.g);         // g
    vertices.push_back(midGreen.b);         // b
    
    vertices.push_back(x + midWidth);       // x
    vertices.push_back(midY);               // y
    vertices.push_back(z + midWidth);       // z
    vertices.push_back(midGreen.r);         // r
    vertices.push_back(midGreen.g);         // g
    vertices.push_back(midGreen.b);         // b
    
    vertices.push_back(x - midWidth);       // x
    vertices.push_back(midY);               // y
    vertices.push_back(z + midWidth);       // z
    vertices.push_back(midGreen.r);         // r
    vertices.push_back(midGreen.g);         // g
    vertices.push_back(midGreen.b);         // b
    
    // Add middle pyramid triangles
    unsigned int midPyramidBase = vertexCount;
    unsigned int midPyramidApex = midPyramidBase;
    unsigned int midPyramidBottomLeft = midPyramidBase + 1;
    unsigned int midPyramidBottomRight = midPyramidBase + 2;
    unsigned int midPyramidTopRight = midPyramidBase + 3;
    unsigned int midPyramidTopLeft = midPyramidBase + 4;
    
    // Four faces of the pyramid
    indices.push_back(midPyramidApex);
    indices.push_back(midPyramidBottomLeft);
    indices.push_back(midPyramidBottomRight);
    
    indices.push_back(midPyramidApex);
    indices.push_back(midPyramidBottomRight);
    indices.push_back(midPyramidTopRight);
    
    indices.push_back(midPyramidApex);
    indices.push_back(midPyramidTopRight);
    indices.push_back(midPyramidTopLeft);
    
    indices.push_back(midPyramidApex);
    indices.push_back(midPyramidTopLeft);
    indices.push_back(midPyramidBottomLeft);
    
    vertexCount += 5;
    
    // Top layer (pointed top)
    float topY = midY + treeHeight * 0.3f;
    float topWidth = midWidth * 0.5f;
    
    // Top pyramid apex
    vertices.push_back(x);                  // x
    vertices.push_back(topY + treeHeight * 0.3f); // y
    vertices.push_back(z);                  // z
    vertices.push_back(lightGreen.r);       // r
    vertices.push_back(lightGreen.g);       // g
    vertices.push_back(lightGreen.b);       // b
    
    // Top pyramid base vertices
    vertices.push_back(x - topWidth);       // x
    vertices.push_back(topY);               // y
    vertices.push_back(z - topWidth);       // z
    vertices.push_back(lightGreen.r);       // r
    vertices.push_back(lightGreen.g);       // g
    vertices.push_back(lightGreen.b);       // b
    
    vertices.push_back(x + topWidth);       // x
    vertices.push_back(topY);               // y
    vertices.push_back(z - topWidth);       // z
    vertices.push_back(lightGreen.r);       // r
    vertices.push_back(lightGreen.g);       // g
    vertices.push_back(lightGreen.b);       // b
    
    vertices.push_back(x + topWidth);       // x
    vertices.push_back(topY);               // y
    vertices.push_back(z + topWidth);       // z
    vertices.push_back(lightGreen.r);       // r
    vertices.push_back(lightGreen.g);       // g
    vertices.push_back(lightGreen.b);       // b
    
    vertices.push_back(x - topWidth);       // x
    vertices.push_back(topY);               // y
    vertices.push_back(z + topWidth);       // z
    vertices.push_back(lightGreen.r);       // r
    vertices.push_back(lightGreen.g);       // g
    vertices.push_back(lightGreen.b);       // b
    
    // Add top pyramid triangles
    unsigned int topPyramidBase = vertexCount;
    unsigned int topPyramidApex = topPyramidBase;
    unsigned int topPyramidBottomLeft = topPyramidBase + 1;
    unsigned int topPyramidBottomRight = topPyramidBase + 2;
    unsigned int topPyramidTopRight = topPyramidBase + 3;
    unsigned int topPyramidTopLeft = topPyramidBase + 4;
    
    // Four faces of the pyramid
    indices.push_back(topPyramidApex);
    indices.push_back(topPyramidBottomLeft);
    indices.push_back(topPyramidBottomRight);
    
    indices.push_back(topPyramidApex);
    indices.push_back(topPyramidBottomRight);
    indices.push_back(topPyramidTopRight);
    
    indices.push_back(topPyramidApex);
    indices.push_back(topPyramidTopRight);
    indices.push_back(topPyramidTopLeft);
    
    indices.push_back(topPyramidApex);
    indices.push_back(topPyramidTopLeft);
    indices.push_back(topPyramidBottomLeft);
    
    vertexCount += 5;
}

void Renderer::setupTerrainMesh(const HeightMap& heightMap) {
    int mapWidth = heightMap.getWidth();
    int mapHeight = heightMap.getHeight();
    
    // Create mesh data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Terrain scale factors - increase these to make the terrain wider
    float horizontalScale = 5.0f;  // Increase from 2.0f to make terrain wider
    float verticalScale = 4.0f;    // Keep the same or adjust as needed
    
    // Generate vertices
    for (int z = 0; z < mapHeight; z++) {
        for (int x = 0; x < mapWidth; x++) {
            float y = heightMap.getHeight(x, z);
            
            // Position - use wider horizontal scale
            vertices.push_back((static_cast<float>(x) / (mapWidth - 1) * 2.0f - 1.0f) * horizontalScale);  // x
            vertices.push_back(y * verticalScale);  // y
            vertices.push_back((static_cast<float>(z) / (mapHeight - 1) * 2.0f - 1.0f) * horizontalScale);  // z
            
            // Color based on terrain type
            glm::vec3 color = getTerrainColor(y);
            vertices.push_back(color.r);  // r
            vertices.push_back(color.g);  // g
            vertices.push_back(color.b);  // b
        }
    }
    
    // Generate indices for triangle strips
    for (int z = 0; z < mapHeight - 1; z++) {
        for (int x = 0; x < mapWidth - 1; x++) {
            int topLeft = z * mapWidth + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * mapWidth + x;
            int bottomRight = bottomLeft + 1;
            
            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    // Add trees on grassy areas
    int vertexCount = mapWidth * mapHeight;  // Current count of vertices
    
    // Define tree distribution parameters
    const float grassLevel = 0.35f;
    const float rockLevel = 0.4f;
    const float treeDensity = 0.9f;  // Controls how many trees to place (lower = more trees)
    
    // Use a simple random number generator
    srand(42);  // Fixed seed for consistent results
    
    for (int z = 2; z < mapHeight - 2; z += 2) {
        for (int x = 2; x < mapWidth - 2; x += 2) {
            float height = heightMap.getHeight(x, z);
            
            // Only place trees on grass/forest level terrain
            if (height >= grassLevel && height < rockLevel) {
                // Random chance to place a tree
                if (rand() / static_cast<float>(RAND_MAX) < treeDensity) {
                    // Calculate actual position
                    float xPos = (static_cast<float>(x) / (mapWidth - 1) * 2.0f - 1.0f) * horizontalScale;
                    float yPos = height * verticalScale; 
                    float zPos = (static_cast<float>(z) / (mapHeight - 1) * 2.0f - 1.0f) * horizontalScale;
                    
                    // Add a tree with random scale between 0.1 and 0.2 (reduced from 0.3-0.5)
                    float treeScale = 0.1f + (rand() / static_cast<float>(RAND_MAX)) * 0.1f;
                    addTreeAt(vertices, indices, xPos, yPos, zPos, treeScale, vertexCount);
                }
            }
        }
    }
    
    // Store the total number of indices
    totalIndicesCount = indices.size();
    
    // Create OpenGL buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    
    // Bind VAO
    glBindVertexArray(vao);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::renderMesh() {
    if (vao == 0) return;
    
    // Use shader program
    glUseProgram(shaderProgram);
    
    // Set up transformations (simple for this example)
    // Model matrix (identity for now)
    float model[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    // View matrix (simple camera looking from above)
    glm::mat4 view = camera.getViewMatrix();  // Use camera view matrix
    
    // Perspective projection
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    float fov = 45.0f * 3.14159f / 180.0f;
    float near = 0.1f;
    float far = 100.0f;
    float tanHalfFov = tan(fov / 2.0f);
    
    float projection[16] = {
        1.0f / (aspect * tanHalfFov), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
        0.0f, 0.0f, -(far + near) / (far - near), -1.0f,
        0.0f, 0.0f, -(2.0f * far * near) / (far - near), 0.0f
    };
    
    // Set uniforms
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
    
    // Draw mesh with all indices including trees
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, totalIndicesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::handleInput(float deltaTime) {
    // Process keyboard input for camera control
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.lookUp(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.lookDown(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.lookLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.lookRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        camera.moveForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        camera.moveBackward(deltaTime);
}

unsigned int Renderer::compileShader(const char* source, unsigned int type) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);
    
    // Check for errors
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetShaderInfoLog(id, length, &length, message);
        
        std::cerr << "Failed to compile shader: " << message << std::endl;
        delete[] message;
        
        glDeleteShader(id);
        return 0;
    }
    
    return id;
}

unsigned int Renderer::createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    unsigned int fs = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    if (vs == 0 || fs == 0) {
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteProgram(program);
        return 0;
    }
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    
    // Check for linking errors
    int result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetProgramInfoLog(program, length, &length, message);
        
        std::cerr << "Failed to link shader program: " << message << std::endl;
        delete[] message;
        
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteProgram(program);
        return 0;
    }
    
    // Cleanup
    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return program;
}