#include "Player.h"

#include "Subsystems/App.h"

void Player::update(const float deltaTime)
{
    m_animationTime += deltaTime * 1.0f; // 0.5 = walking speed
    // TODO If (x/z) velocity then stand still animation (or move to idle!)
    if (m_velocity.z == 0) m_animationTime = 0.25f;
    // TODO If y velocity < 0 then falling etc etc

    m_rotation.y = m_yaw;

    doPlayerMovement(deltaTime);
    checkMapCollision(deltaTime);
}

void Player::checkMapCollision(const float deltaTime)
{
    glm::vec3 newPosition = m_position + m_velocity;

    //TODO
    m_position = newPosition;
}

void Player::doPlayerMovement(const float deltaTime)
{
    auto movement = App::get()->getInput()->getMovement();
    m_velocity = getForwardVector() * movement.y + getLeftVector() * movement.x;
    m_velocity *= deltaTime * 4.0f;
}
glm::vec3 Player::getForwardVector()
{
    glm::vec3 forward;
    forward.x = sin(glm::radians(m_yaw));
    forward.y = 0.0f;
    forward.z = cos(glm::radians(m_yaw));
    return glm::normalize(forward);
}

glm::vec3 Player::getLeftVector()
{
    const auto forward = getForwardVector();
    return glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forward));
}
