#include "InputSystem.h"
#include "App.h"
#include <iostream>

void InputSystem::initializeGamepads()
{
    if (!SDL_HasGamepad())
    {
        SDL_Log("No Gamepads connected");
        m_bGamepad = false;
        return;
    }

    int count;
    auto joystickIDs = SDL_GetGamepads(&count);
    if (!joystickIDs)
    {
        SDL_Log("Unable to get joystick IDs");
        m_bGamepad = false;
        return;
    }

    m_gamepad = SDL_OpenGamepad(joystickIDs[0]);
    SDL_Log("%s connected", SDL_GetGamepadName(m_gamepad));
    // SDL_RumbleGamepad(m_gamepad, 0xffff, 0xffff, 500);

    SDL_free(joystickIDs);

    // TODO Deal with unplugging and plugging in during game, may need to move some of this code to a separate function
    // TODO Allow user to select which Gamepad to use in settings?

    // Set the size of button Array!
    m_buttonStates.resize(SDL_GAMEPAD_BUTTON_COUNT, false);

    // It looks like we are all good
    m_bGamepad = true;
}

void InputSystem::update(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
        onButtonChange(event);
        break;

    case SDL_EVENT_MOUSE_MOTION:
        onMouseMove(event);
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        onMouseButtonChange(event);
        break;

    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        onKeyChange();
        break;
    default:
        break;
    }
}

void InputSystem::clean() const
{
    if (m_bGamepad)
    {
        SDL_CloseGamepad(m_gamepad);
    }
}

glm::vec2 InputSystem::getMovement() const
{
    glm::vec2 newVelocity(0.0f, 0.0f);

    if (getKeyDown(App::get()->getSettings()->m_keyboard.RIGHT) || getButtonDown(
        App::get()->getSettings()->m_gamepad.RIGHT))
    {
        newVelocity.x = 1.0f;
    }
    else if (getKeyDown(App::get()->getSettings()->m_keyboard.LEFT) || getButtonDown(
        App::get()->getSettings()->m_gamepad.LEFT))
    {
        newVelocity.x = -1.0f;
    }

    if (getKeyDown(App::get()->getSettings()->m_keyboard.DOWN) || getButtonDown(
        App::get()->getSettings()->m_gamepad.DOWN))
    {
        newVelocity.y = 1.0f;
    }
    else if (getKeyDown(App::get()->getSettings()->m_keyboard.UP) || getButtonDown(
        App::get()->getSettings()->m_gamepad.UP))
    {
        newVelocity.y = -1.0f;
    }

    if (glm::length(newVelocity) != 0)
    {
        newVelocity = glm::normalize(newVelocity);
    }

    return newVelocity;
}

glm::vec2 InputSystem::getMouseMovement() const
{
    return m_mouseMovement;
}

bool InputSystem::getAction(const actions action) const
{
    switch (action)
    {
    case actions::MENU:
        return getKeyDown(App::get()->getSettings()->m_keyboard.MENU) || getButtonDown(
            App::get()->getSettings()->m_gamepad.MENU);
    case actions::ATTACK:
        return getKeyDown(App::get()->getSettings()->m_keyboard.ATTACK) || getButtonDown(
            App::get()->getSettings()->m_gamepad.ATTACK);
    case actions::SECONDARY_ATTACK:
        return getKeyDown(App::get()->getSettings()->m_keyboard.SECONDARY_ATTACK) || getButtonDown(
            App::get()->getSettings()->m_gamepad.SECONDARY_ATTACK);
    case actions::CONFIRM:
        return getKeyDown(App::get()->getSettings()->m_keyboard.CONFIRM) || getButtonDown(
            App::get()->getSettings()->m_gamepad.CONFIRM);
    case actions::CANCEL:
        return getKeyDown(App::get()->getSettings()->m_keyboard.CANCEL) || getButtonDown(
            App::get()->getSettings()->m_gamepad.CANCEL);
    }
    return false;
}

bool InputSystem::getButtonDown(const int button) const
{
    // Allow us to check if a button is pressed without first checking if there is an active gamepad
    if (!m_bGamepad)
        return false;

    return m_buttonStates[button];
}

std::string InputSystem::getButtonLabel(const SDL_GamepadButton button) const
{
    if (!m_bGamepad)
        return "";

    switch (SDL_GetGamepadButtonLabel(m_gamepad, button))
    {
    case SDL_GAMEPAD_BUTTON_LABEL_A:
        return "A";
    case SDL_GAMEPAD_BUTTON_LABEL_B:
        return "B";
    case SDL_GAMEPAD_BUTTON_LABEL_X:
        return "X";
    case SDL_GAMEPAD_BUTTON_LABEL_Y:
        return "Y";
    case SDL_GAMEPAD_BUTTON_LABEL_CROSS:
        return "X";
    case SDL_GAMEPAD_BUTTON_LABEL_CIRCLE:
        return "O";
    case SDL_GAMEPAD_BUTTON_LABEL_SQUARE:
        return "Square";
    case SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE:
        return "Triangle";
    default:
        return "Non face key!";
    }
}

void InputSystem::initializeMouse()
{
    for (int i = 0; i < 3; i++)
    {
        m_mouseButtonStates.push_back(false);
    }
    m_mousePosition = glm::vec2(0.0f, 0.0f);
}

bool InputSystem::getMouseButtonState(mouseButtons buttonNumber)
{
    return m_mouseButtonStates[static_cast<int>(buttonNumber)];
}

void InputSystem::resetMouseMovement()
{
    m_mouseMovement = {0.0f, 0.0f};
}

glm::vec2 InputSystem::getMousePosition() const
{
    return m_mousePosition;
}

bool InputSystem::getKeyDown(const int key) const
{
    if (!m_keyStates)
        return false;
    return m_keyStates[key];
}

void InputSystem::onButtonChange(const SDL_Event& event)
{
    m_buttonStates[event.gbutton.button] = event.gbutton.down;
}

void InputSystem::onKeyChange()
{
    m_keyStates = SDL_GetKeyboardState(nullptr);
}

void InputSystem::onMouseMove(const SDL_Event& event)
{
    constexpr float scale = 1.0f;
    // TODO Make this a setting?
    m_mousePosition.x = (event.motion.x / scale);
    m_mousePosition.y = (event.motion.y / scale);
    m_mouseMovement.x = (event.motion.xrel / scale);
    m_mouseMovement.y = (event.motion.yrel / scale);
}

void InputSystem::onMouseButtonChange(const SDL_Event& event)
{
    const bool state = event.button.down;
    // TODO Refactor this like onButtonChange
    switch (event.button.button)
    {
    case SDL_BUTTON_LEFT:
        m_mouseButtonStates[static_cast<int>(mouseButtons::LEFT)] = state;
        break;
    case SDL_BUTTON_MIDDLE:
        m_mouseButtonStates[static_cast<int>(mouseButtons::MIDDLE)] = state;
        break;
    case SDL_BUTTON_RIGHT:
        m_mouseButtonStates[static_cast<int>(mouseButtons::RIGHT)] = state;
        break;
    default:
        break;
    }
}
