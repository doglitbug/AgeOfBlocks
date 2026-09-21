#pragma once
#include "../Observers.h"
#include <SDL3/SDL.h>

// Any change to this struct will probably require deleting the settings already on disk!
// TODO Split into Audio/Video/Keyboard/Gamepad/General (all inside another struct?)
struct settings
{
    bool titleMusicEnabled;
    int titleMusicVolume;
    bool gameMusicEnabled;
    int gameMusicVolume;
    int gameVolume;
    bool fullScreen;
    int screenWidth;
    int screenHeight;
};

/// @brief Input bindings, either keyboard or gamepad
/// Idea is that we can ask if m_keyboard.ATTACK or m_gamepad.ATTACK has been pushed to know if we need to attack!
struct bindings
{
    int UP;
    int DOWN;
    int LEFT;
    int RIGHT;
    int MENU;
    int ATTACK;
    int SECONDARY_ATTACK;
    int CONFIRM;
    int CANCEL;
};

enum class actions {
    MENU,
    ATTACK,
    SECONDARY_ATTACK,
    CONFIRM,
    CANCEL
};

class Settings : public ISubject
{
public:
    Settings();
    ~Settings() override = default;

    void load();
    void save();
    void reset();

    // Audio
    bool getTitleMusicEnabled() const;
    void setTitleMusicEnabled(bool enabled);
    int getTitleMusicVolume() const;
    void setTitleMusicVolume(int volume);

    bool getGameMusicEnabled() const;
    void setGameMusicEnabled(bool enabled);
    int getGameMusicVolume() const;
    void setGameMusicVolume(int volume);

    int getGameVolume() const;
    void setGameVolume(int volume);

    // Video
    bool getFullScreen() const;
    void setFullScreen(bool enabled);
    void setResolution(int width, int height);
    glm::ivec2 getResolution() const;

    // Input
    /// @see https://wiki.libsdl.org/SDL3/SDL_Scancode
    bindings m_keyboard = {
        SDL_SCANCODE_W,
        SDL_SCANCODE_S,
        SDL_SCANCODE_A,
        SDL_SCANCODE_D,
        SDL_SCANCODE_ESCAPE,
        SDL_SCANCODE_RETURN,
        SDL_SCANCODE_SPACE,
        SDL_SCANCODE_RETURN,
        SDL_SCANCODE_BACKSPACE};

    /// @see https://wiki.libsdl.org/SDL3/SDL_GamepadButton
    bindings m_gamepad = {
        SDL_GAMEPAD_BUTTON_DPAD_UP,
        SDL_GAMEPAD_BUTTON_DPAD_DOWN,
        SDL_GAMEPAD_BUTTON_DPAD_LEFT,
        SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
        SDL_GAMEPAD_BUTTON_START,
        SDL_GAMEPAD_BUTTON_SOUTH,
        SDL_GAMEPAD_BUTTON_EAST,
        SDL_GAMEPAD_BUTTON_WEST,
        SDL_GAMEPAD_BUTTON_NORTH};
private:
    settings m_settings{};
};