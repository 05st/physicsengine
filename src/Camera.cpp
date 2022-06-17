#include "Camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    
    this->front = glm::vec3(0.0f, 0.0f, -1.0f);
    this->speed = 10.0f;
    this->sensitivity = 0.1f;
    this->fov = 70.0f;

    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(this->position, this->position + this->front, this->up);
}

void Camera::processKeyboard(CameraMovement dir, float dt) {
    float v = this->speed * dt;
    if (dir == FORWARD)
        this->position += this->front * v;
    if (dir == BACKWARD)
        this->position -= this->front * v;
    if (dir == LEFT)
        this->position -= this->right * v;
    if (dir == RIGHT)
        this->position += this->right * v;
}

void Camera::processMouse(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= this->sensitivity;
    yOffset *= this->sensitivity;

    this->yaw += xOffset;
    this->pitch += yOffset;

    if (constrainPitch) {
        if (this->pitch > 89.0f)
            this->pitch = 89.0f;
        if (this->pitch < -89.0f)
            this->pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::processScroll(float yOffset) {
    fov -= yOffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 70.0f)
        fov = 70.0f;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    front.y = sin(glm::radians(this->pitch));
    front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->front = glm::normalize(front);
    this->right = glm::normalize(glm::cross(this->front, this->worldUp));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}