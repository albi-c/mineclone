#include "util/ray.hpp"

Ray::Ray(const glm::vec3& pos, const glm::vec3& dir)
    : pos(pos), dir(glm::normalize(dir)), dist(0.0f) {}

void Ray::step(float amount) {
    pos += dir * amount;
    dist += amount;
}
glm::vec3 Ray::position() {
    return pos;
}
float Ray::distance() {
    return dist;
}
