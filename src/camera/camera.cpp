#include "camera/camera.hpp"

void CameraHandlers::framebuffer_resize_event_handler(const EventFramebufferResize& e) {
    Camera::resize(e.width, e.height);
}

void Camera::init(float fov) {
    Camera::fov = fov;

    EventManager::listen(handlers.framebuffer_resize_event_queue);

    update_vectors();
}

glm::mat4 Camera::view_matrix() {
    return glm::lookAt(pos, pos + front, up);
}
glm::mat4 Camera::proj_matrix() {
    handlers.framebuffer_resize_event_queue.process();
    return glm::perspective(fov, (float)width / (float)height, near_plane, far_plane);
}
glm::mat4 Camera::ortho_matrix() {
    handlers.framebuffer_resize_event_queue.process();
    return glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
}

frustum::Frustum Camera::frustum() {
    frustum::Frustum frustum;
    const float halfVSide = far_plane * tanf(fov * .5f);
    const float halfHSide = halfVSide * ((float)width / (float)height);
    const glm::vec3 frontMultFar = far_plane * front;

    frustum.near = {pos + near_plane * front, front};
    frustum.far = {pos + frontMultFar, -front};
    frustum.right = {pos, glm::cross(up, frontMultFar + right * halfHSide)};
    frustum.left = {pos, glm::cross(frontMultFar - right * halfHSide, up)};
    frustum.top = {pos, glm::cross(right, frontMultFar - up * halfVSide)};
    frustum.bottom = {pos, glm::cross(frontMultFar + up * halfVSide, right)};

    return frustum;
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
    rot.x += x * 0.1f;
    rot.y += y * 0.1f;

    if (rot.y > 89.0f)
        rot.y = 89.0f;
    if (rot.y < -89.0f)
        rot.y = -89.0f;
    
    update_vectors();
}

void Camera::resize(int width, int height) {
    Camera::width = width;
    Camera::height = height;
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
