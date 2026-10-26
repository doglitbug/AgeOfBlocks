#include "Player.h"

#include "Subsystems/App.h"

void Player::update(const float deltaTime)
{
    m_animationTime += deltaTime * 1.0f; // 0.5 = walking speed
    // TODO If (x/z) velocity then stand still animation (or move to idle!)
    if (m_velocity.z == 0) m_animationTime = 0.25f;
    // TODO If y velocity < 0 then falling etc etc

    m_rotation.y = m_yaw;
}

void Player::doPlayerMovement(const float deltaTime)
{
    auto movement = App::get()->getInput()->getMovement();
    m_velocity = getForwardVector() * movement.y + getLeftVector() * movement.x;
    m_velocity *= deltaTime * 4.0f; //TODO Run speed/walk speed
}

void Player::checkMapCollisions(float deltaTime, map& map)
{
    glm::vec3 newPosition = m_position + m_velocity * deltaTime;//Assume y = 0?
    // Collision on X axis
    if (m_velocity.x < 0)
    {
        if (!map.isWalkable(newPosition.x + m_hitBox->x, m_position.getY() + m_hitBox->y) ||
            !m_pCollisionLayer.isWalkable(newPosition.getX() + m_hitBox->x,
                                          m_position.getY() + m_hitBox->y + m_hitBox->h))
        {
            m_velocity.x = 0;
        }
    }
    else if (m_velocity.x > 0)
    {
        if (!m_pCollisionLayer.isWalkable(newPosition.getX() + m_hitBox->x + m_hitBox->w,
                                          m_position.getY() + m_hitBox->y) ||
            !m_pCollisionLayer.isWalkable(newPosition.getX() + m_hitBox->x + m_hitBox->w,
                                          m_position.getY() + m_hitBox->y + m_hitBox->h))
        {
            m_velocity.setX(0);
        }
    }

    m_position += m_velocity;
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
