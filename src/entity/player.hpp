#pragma once

#include "entity/entity.hpp"

#include "camera/camera.hpp"

enum class PlayerMoveDirection {
    FORWARD = 0x1,
    BACKWARD = 0x2,
    LEFT = 0x4,
    RIGHT = 0x8,
    UP = 0x10,
    DOWN = 0x20
};

class Player : public Entity {
public:
    void render();

    void move(char directions, float dt);
    void rotate(float x, float y);

    glm::vec3 get_eye_pos() const;
};
