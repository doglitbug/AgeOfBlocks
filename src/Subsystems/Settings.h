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
    std::string language;
};

enum class actions {
    FORWARD,
    BACKWARD,
    STRAFE_LEFT,
    STRAFE_RIGHT,
    TURN_LEFT,
    TURN_RIGHT,
    MENU,
    TOGGLE_GRID,
    COUNT
};

struct bindings {
    int keys[static_cast<size_t>(actions::COUNT)] = {}; // Default initializes all to 0

    int get(actions action) const {
        return keys[static_cast<size_t>(action)];
    }

    void set(actions action, const int newKey) {
        keys[static_cast<size_t>(action)] = newKey;
    }
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

    // Language
    /**
     * BCP 47 standard country codes
     * @see https://en.wikipedia.org/wiki/IETF_language_tag
     * @return Current language code
     */
    std::string getLanguage() const;
    /**
     * BCP 47 standard country codes
     * @param language
     */
    void setLanguage(const std::string& language);

    // Input
    /// @see https://wiki.libsdl.org/SDL3/SDL_Scancode
    /// @see
    bindings m_keyboard = {
        SDL_SCANCODE_W,
        SDL_SCANCODE_S,
        SDL_SCANCODE_A,
        SDL_SCANCODE_D,
        SDL_SCANCODE_Q,
        SDL_SCANCODE_E,
        SDL_SCANCODE_ESCAPE,
        SDL_SCANCODE_G
    };

    /// @see https://wiki.libsdl.org/SDL3/SDL_GamepadButton
    bindings m_gamepad = {};
private:
    settings m_settings{};
};