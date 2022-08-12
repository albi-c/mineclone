#include "camera.hpp"

Camera::Camera(float fov)
    : fov(fov) {
    
    update_vectors();
}

glm::mat4 Camera::view_matrix() {
    return glm::lookAt(pos, pos + front, up);
}
glm::mat4 Camera::proj_matrix() {
    return glm::perspective(fov, (float)width / (float)height, 0.1f, 1000.0f);
}
glm::mat4 Camera::ortho_matrix() {
    return glm::ortho(0.0f, (float)width, 0.0f, (float)height, -10.0f, 10.0f);
}

void Camera::move(char directions, float dt) {
    glm::vec3 offset = glm::vec3(0);
    if (directions & (int)CameraMoveDirection::FORWARD)
        offset += front;
    if (directions & (int)CameraMoveDirection::BACKWARD)
        offset -= front;
    if (directions & (int)CameraMoveDirection::LEFT)
        offset -= right;
    if (directions & (int)CameraMoveDirection::RIGHT)
        offset += right;
    
    offset.y = 0;
    if (glm::length(offset) != 0.0f)
        offset = glm::normalize(offset);

    if (directions & (int)CameraMoveDirection::UP)
        offset.y += 1;
    if (directions & (int)CameraMoveDirection::DOWN)
        offset.y -= 1;
    
    pos += offset * dt;
}
void Camera::rotate(float x, float y) {
    rot.x += x * 0.1;
    rot.y += y * 0.1;

    if (rot.y > 89.0f)
        rot.y = 89.0f;
    if (rot.y < -89.0f)
        rot.y = -89.0f;
    
    update_vectors();
}

void Camera::resize(int width, int height) {
    this->width = width;
    this->height = height;
}

void Camera::update_vectors() {
    glm::vec3 front_;
    front_.x = cos(glm::radians(rot.x)) * cos(glm::radians(rot.y));
    front_.y = sin(glm::radians(rot.y));
    front_.z = sin(glm::radians(rot.x)) * cos(glm::radians(rot.y));
    front = glm::normalize(front_);

    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}
