#pragma once

#include <variant>
#include <string>
#include <algorithm>
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
    virtual void addObserver(IObserver *observer)
    {
        observers.push_back(observer);
    };
    virtual void removeObserver(IObserver *observer)
    {
        std::erase(observers, observer);
    };
    virtual void notifyObservers(const std::string &message, const MyType newValue)
    {
        for (IObserver *observer : observers)
        {
            if (observer)
            {
                observer->onNotify(message, newValue); // Synchronous broadcast
            }
        }
    };

    std::vector<IObserver *> observers;
};
