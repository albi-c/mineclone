#pragma once

#include "lib/opengl.hpp"
#include "frustum.hpp"

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
    static inline glm::vec3 pos = glm::vec3(0.0f);
    static inline glm::vec2 rot = glm::vec3(0.0f);
    static inline float fov = 0.0f;

    static inline glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

    static inline float near_plane = 0.1f;
    static inline float far_plane = 1000.0f;

    static void init(float fov);

    static glm::mat4 view_matrix();
    static glm::mat4 proj_matrix();
    static glm::mat4 ortho_matrix();

    static frustum::Frustum frustum();

    static void move(char directions, float dt);
    static void rotate(float x, float y);

    static void resize(int width, int height);

private:
    static inline glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    static inline glm::vec3 right;
    static inline glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);

    static inline int width, height;

    static void update_vectors();
};
