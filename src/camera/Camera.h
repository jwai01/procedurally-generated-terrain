#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(const glm::vec3& position = glm::vec3(0.0f, 3.0f, 5.0f));
    
    // Get view matrix
    glm::mat4 getViewMatrix() const;
    
    // Movement
    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    
    // Look direction
    void lookUp(float deltaTime);
    void lookDown(float deltaTime);
    void lookLeft(float deltaTime);
    void lookRight(float deltaTime);
    
    // Getters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    // Camera angles
    float yaw;
    float pitch;
    
    // Movement speed
    float movementSpeed;
    float rotationSpeed;
    
    // Update camera vectors based on angles
    void updateCameraVectors();
};