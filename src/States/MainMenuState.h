#pragma once

#include "BaseState.h"

class MainMenuState : public BaseState
{
public:
    void onEnter() override;
    void update(float deltaTime) override;
    void render() override;
    void onExit() override;
};
