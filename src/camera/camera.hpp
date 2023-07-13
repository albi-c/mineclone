#pragma once

#include "opengl.hpp"
#include "camera/frustum.hpp"
#include "window/event.hpp"

enum class CameraMoveDirection {
    FORWARD = 0x1,
    BACKWARD = 0x2,
    LEFT = 0x4,
    RIGHT = 0x8,
    UP = 0x10,
    DOWN = 0x20
};

class Camera;
struct CameraHandlers {
    static void framebuffer_resize_event_handler(const EventFramebufferResize& e);
    FunctionEventQueue<EventFramebufferResize> framebuffer_resize_event_queue = \
        FunctionEventQueue<EventFramebufferResize>(CameraHandlers::framebuffer_resize_event_handler);
};

class Camera {
public:
    static inline glm::vec3 pos = glm::vec3(0.0f);
    static inline glm::vec2 rot = glm::vec3(0.0f);
    static inline float fov = 0.0f;

    static inline glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    static inline glm::vec3 right;

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

    static void update_vectors();

private:
    static inline glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    static inline glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);

    static inline int width, height;

    static inline CameraHandlers handlers;
};
