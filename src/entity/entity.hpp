#pragma once

#include "opengl.hpp"

class Entity {
public:
    glm::vec3 pos;
    glm::vec2 rot;

    virtual void update(float dt) {}
    virtual void render() {}

    void move(const glm::vec3& motion);
    void rotate(const glm::vec2& rotation);
};
