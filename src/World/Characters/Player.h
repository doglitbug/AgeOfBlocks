#pragma once
#include "ObjectAnimated.h"

class Player : public ObjectAnimated
{
public:
    void update(float deltaTime);
    void checkMapCollision(float deltaTime);
    float m_yaw = 0.0f;

private:
    void doPlayerMovement(const float deltaTime);
    glm::vec3 getForwardVector();
    glm::vec3 getLeftVector();
private:
};
