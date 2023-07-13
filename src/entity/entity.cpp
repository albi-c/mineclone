#include "entity/entity.hpp"

void Entity::move(const glm::vec3& motion) {
    pos += motion;
}
void Entity::rotate(const glm::vec2& rotation) {
    rot += rotation;
}
