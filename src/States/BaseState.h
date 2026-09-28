#pragma once

class BaseState
{
public:
    virtual ~BaseState() = default;
    virtual void onEnter() =0;
    virtual void update(float deltaTime) =0;
    virtual void render() =0;
    virtual void onExit() =0;
};
