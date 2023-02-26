#include "player.hpp"

void Player::render() {
    //
}

void Player::move(char directions, float dt) {
    glm::vec3 offset = glm::vec3(0);
    if (directions & (int)PlayerMoveDirection::FORWARD)
        offset += Camera::front;
    if (directions & (int)PlayerMoveDirection::BACKWARD)
        offset -= Camera::front;
    if (directions & (int)PlayerMoveDirection::LEFT)
        offset -= Camera::right;
    if (directions & (int)PlayerMoveDirection::RIGHT)
        offset += Camera::right;
    
    offset.y = 0;
    if (glm::length(offset) != 0.0f)
        offset = glm::normalize(offset);

    if (directions & (int)PlayerMoveDirection::UP)
        offset.y += 1;
    if (directions & (int)PlayerMoveDirection::DOWN)
        offset.y -= 1;
    
    pos += offset * dt;
    Camera::pos = get_eye_pos();
}
void Player::rotate(float x, float y) {
    rot.x += x * 0.1f;
    rot.y += y * 0.1f;

    if (rot.y > 89.0f)
        rot.y = 89.0f;
    if (rot.y < -89.0f)
        rot.y = -89.0f;
    
    Camera::rot = rot;
    
    Camera::update_vectors();
}

glm::vec3 Player::get_eye_pos() const {
    return pos + glm::vec3(0.0f, 1.64f, 0.0f);
}
