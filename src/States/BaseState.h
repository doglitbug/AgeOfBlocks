#pragma once

class BaseState
{
public:
    virtual ~BaseState() = default;
    virtual void onEnter(){};
    virtual void update(float deltaTime){};
    virtual void render(){};
    virtual void onExit(){};
};
