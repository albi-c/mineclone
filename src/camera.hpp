#pragma once

#include "graphics/opengl.hpp"

enum class CameraMoveDirection {
    FORWARD = 0x1,
    BACKWARD = 0x2,
    LEFT = 0x4,
    RIGHT = 0x8,
    UP = 0x10,
    DOWN = 0x20
};

class Camera {
public:
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec2 rot = glm::vec3(0.0f);
    float fov = 0.0f;

    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

    Camera(float fov);

    glm::mat4 view_matrix();
    glm::mat4 proj_matrix();
    glm::mat4 ortho_matrix();

    void move(char directions, float dt);
    void rotate(float x, float y);

    void resize(int width, int height);

private:
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right;
    glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);

    int width, height;

    void update_vectors();
};
