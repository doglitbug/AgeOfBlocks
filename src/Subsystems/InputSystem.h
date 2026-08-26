#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "Settings.h"

enum class mouseButtons
{
    LEFT = 0,
    MIDDLE = 1,
    RIGHT = 2
};

class InputSystem
{
public:
    InputSystem() = default;
    ~InputSystem() = default;

    void update(const SDL_Event &event);
    void clean() const;

    /// @return Normalized movement vector based on user input
    [[nodiscard]] glm::vec2 getMovement() const;

    /// @brief See if the action is currently being performed via keyboard/controller
    /// @param action actions.MENU/ATTACK etc
    /// @return boolean
    bool getAction(actions action) const;

    // Joystick Handling
    void initializeGamepads();
    bool gamepadInUse() const { return m_bGamepad; }

    /// @param button
    /// @return
    [[nodiscard]] std::string getButtonLabel(SDL_GamepadButton button) const;

    // Mouse handling
    void initializeMouse();
    bool getMouseButtonState(mouseButtons buttonNumber);
    void resetMouseMovement();
    [[nodiscard]] glm::vec2 getMousePosition() const;
    [[nodiscard]] glm::vec2 getMouseMovement() const;

private:
    // Mouse
    glm::vec2 m_mousePosition;
    glm::vec2 m_mouseMovement;
    std::vector<bool> m_mouseButtonStates;
    void onMouseMove(const SDL_Event &event);
    void onMouseButtonChange(const SDL_Event &event);

    // Keyboard
    const bool *m_keyStates;
    void onKeyChange();
    [[nodiscard]] bool getKeyDown(int key) const;

    // Joysticks
    /// @brief are we using a GamePad?
    bool m_bGamepad;
    [[nodiscard]] bool getButtonDown(int button) const;
    SDL_Gamepad *m_gamepad;
    std::vector<bool> m_buttonStates;
    void onButtonChange(const SDL_Event &event);
};