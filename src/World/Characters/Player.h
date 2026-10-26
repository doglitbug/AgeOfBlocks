#pragma once
#include "ObjectAnimated.h"
#include "World/map.h"

class Player : public ObjectAnimated
{
public:
    void update(float deltaTime);
    float m_yaw = 0.0f;
    void doPlayerMovement(float deltaTime);
    void checkMapCollisions(float deltaTime, map& playerMap);

private:
    glm::vec3 getForwardVector();
    glm::vec3 getLeftVector();
};
