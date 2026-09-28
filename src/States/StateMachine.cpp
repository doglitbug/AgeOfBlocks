#include "StateMachine.h"

#include <SDL3/SDL_log.h>

void StateMachine::update(float deltaTime) {
    m_gameStateStack.back()->update(deltaTime);
    if (m_requestedChangeType != changeType::NOCHANGE) {
        changeState();
    }
}

void StateMachine::render() const
{
    m_gameStateStack.back()->render();
}

void StateMachine::registerState(const std::string& stateID, BaseState *pState) {
    //Is this the first state, if so, set as default
    if (m_gameStateStack.empty())
    {
        m_gameStateStack.push_back(pState);
        m_gameStateStack.back()->onEnter();
    }
    m_gameStates[stateID] = pState;
}

void StateMachine::setInitialState(const std::string& stateID) {
    if (m_gameStates[stateID]) {
        m_gameStateStack.push_back(m_gameStates[stateID]);
        m_gameStateStack.back()->onEnter();
    } else {
        SDL_Log("Initial state %s not found", stateID.c_str());
    }
}

void StateMachine::changeState(const std::string& stateID) {
    if (m_gameStates[stateID]) {
        m_requestedState = stateID;
        m_requestedChangeType = changeType::CHANGE;
    } else {
        SDL_Log("State %s not found", stateID.c_str());
    }
}

void StateMachine::pushState(const std::string& stateID) {
    if(m_gameStates[stateID]) {
        m_requestedState = stateID;
        m_requestedChangeType = changeType::PUSH;
    } else {
        SDL_Log("State %s not found", stateID.c_str());
    }
}

void StateMachine::popState() {
    m_requestedChangeType = changeType::POP;
}

void StateMachine::changeState() {
    switch (m_requestedChangeType) {
        case changeType::CHANGE:
            //Remove current states
            for (BaseState *gs: m_gameStateStack) {
                gs->onExit();
            }
            m_gameStateStack.clear();

            //Add new one
            m_gameStateStack.push_back(m_gameStates[m_requestedState]);
            m_gameStateStack.back()->onEnter();
            break;
        case changeType::PUSH:
            m_gameStateStack.push_back(m_gameStates[m_requestedState]);
            m_gameStateStack.back()->onEnter();
            break;
        case changeType::POP:
            m_gameStateStack.back()->onExit();
            m_gameStateStack.pop_back();
            break;
        case changeType::NOCHANGE:
            break;
    }
    m_requestedChangeType = changeType::NOCHANGE;
}