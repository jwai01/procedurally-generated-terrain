#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& position) 
    : position(position),
      worldUp(0.0f, 1.0f, 0.0f),
      yaw(-90.0f),
      pitch(0.0f),
      movementSpeed(5.0f),
      rotationSpeed(50.0f)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::moveForward(float deltaTime) {
    position += front * movementSpeed * deltaTime;
}

void Camera::moveBackward(float deltaTime) {
    position -= front * movementSpeed * deltaTime;
}

void Camera::lookUp(float deltaTime) {
    pitch += rotationSpeed * deltaTime;
    // Constrain pitch to avoid flipping
    if (pitch > 89.0f) pitch = 89.0f;
    updateCameraVectors();
}

void Camera::lookDown(float deltaTime) {
    pitch -= rotationSpeed * deltaTime;
    // Constrain pitch to avoid flipping
    if (pitch < -89.0f) pitch = -89.0f;
    updateCameraVectors();
}

void Camera::lookLeft(float deltaTime) {
    yaw -= rotationSpeed * deltaTime;
    updateCameraVectors();
}

void Camera::lookRight(float deltaTime) {
    yaw += rotationSpeed * deltaTime;
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    // Calculate new front vector
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    
    front = glm::normalize(newFront);
    
    // Recalculate right and up vectors
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}