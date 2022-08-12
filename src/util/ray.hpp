#pragma once

#include "lib/opengl.hpp"

class Ray {
public:
    Ray(const glm::vec3& pos, const glm::vec3& dir);

    void step(float amount);
    glm::vec3 position();
    float distance();

private:
    glm::vec3 pos;
    glm::vec3 dir;
    float dist;
};
