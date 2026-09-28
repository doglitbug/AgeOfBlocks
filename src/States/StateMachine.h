#pragma once

#include <vector>
#include <map>
#include <string>

#include "BaseState.h"

class StateMachine {
public:
    void update(float deltaTime);

    void render() const;

    void registerState(const std::string& stateID, BaseState *pState);
    void setInitialState(const std::string& stateID);
    void changeState(const std::string& stateID);
    void pushState(const std::string& stateID);
    void popState();

private:
    std::map<std::string, BaseState *> m_gameStates;
    std::vector<BaseState *> m_gameStateStack;

    enum class changeType {
        NOCHANGE,
        CHANGE,
        PUSH,
        POP
    };

    std::string m_requestedState;
    changeType m_requestedChangeType = changeType::NOCHANGE;

    void changeState();

    ~StateMachine() {
        for (auto it = m_gameStates.cbegin(); it != m_gameStates.cend() /* not hoisted */; /* no increment */) {
            delete it->second;
            m_gameStates.erase(it++);
        }
    };
};
