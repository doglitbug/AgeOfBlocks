#pragma once

#include <SDL3/SDL_rect.h>
#include <variant>
#include <string>
#include <glm/glm.hpp>

using MyType = std::variant<int, float, bool, glm::ivec2>;
class IObserver
{
public:
    virtual ~IObserver() = default;
    virtual void onNotify(const std::string &message, MyType newValue) = 0;
};

class ISubject
{
public:
    virtual ~ISubject() = default;
    virtual void addObserver(IObserver *observer) = 0;
    virtual void removeObserver(IObserver *observer) = 0;
    virtual void notifyObservers(const std::string &message, MyType newValue) = 0;
};