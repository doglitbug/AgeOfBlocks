#include <iostream>
#include <fstream>

#include "Settings.h"
#include "App.h"

Settings::Settings()
{
    load();
    reset();
    //save();
}
void Settings::load()
{
    // TODO Refactor these into an object for RAII
    // TODO Maybe a try catch and load defaults if error?
    std::ifstream inFile;

    // Get file location
    char *filepath = SDL_GetPrefPath("0x00101010", "Age of Blocks");

    inFile.open(std::string(filepath) + "settings", std::ios::binary);

    if (inFile)
    {
        // TODO Catch any errors?
        inFile.read(reinterpret_cast<char *>(&m_settings), sizeof(m_settings));
    }
    else
    {
        reset();
    }

    inFile.close();
    SDL_free(filepath);
}
void Settings::save()
{
    // TODO Refactor these into an object for RAII
    std::ofstream outFile;

    // Get file location
    char *filepath = SDL_GetPrefPath("0x00101010", "Age of Blocks");

    outFile.open(std::string(filepath) + "settings", std::ios::binary);

    if (outFile)
    {
        outFile.write(reinterpret_cast<char *>(&m_settings), sizeof(m_settings));
    }
    else
    {
        SDL_Log("Error writing to settings file");
    }

    outFile.close();
    SDL_free(filepath);
}

void Settings::reset()
{
    setTitleMusicEnabled(true);
    setTitleMusicVolume(5);
    setGameMusicEnabled(true);
    setGameMusicVolume(5);
    setGameVolume(75);
    setFullScreen(false);
    setResolution(800, 600);

    // TODO Will probably need a dedicated function for resetting keybindings?
}

// region Audio
bool Settings::getTitleMusicEnabled() const { return m_settings.titleMusicEnabled; }
void Settings::setTitleMusicEnabled(const bool enabled)
{
    m_settings.titleMusicEnabled = enabled;
    notifyObservers("TITLE_MUSIC_ENABLED", enabled);
}
int Settings::getTitleMusicVolume() const { return m_settings.titleMusicVolume; };
void Settings::setTitleMusicVolume(const int volume)
{
    m_settings.titleMusicVolume = volume;
    notifyObservers("TITLE_MUSIC_VOLUME", volume);
};

bool Settings::getGameMusicEnabled() const { return m_settings.gameMusicEnabled; };
void Settings::setGameMusicEnabled(const bool enabled)
{
    m_settings.gameMusicEnabled = enabled;
    notifyObservers("GAME_MUSIC_ENABLED", enabled);
}

int Settings::getGameMusicVolume() const { return m_settings.gameMusicVolume; };
void Settings::setGameMusicVolume(const int volume)
{
    m_settings.gameMusicVolume = volume;
    notifyObservers("GAME_MUSIC_VOLUME", volume);
};

int Settings::getGameVolume() const { return m_settings.gameVolume; };
void Settings::setGameVolume(const int volume)
{
    m_settings.gameVolume = volume;
    notifyObservers("GAME_VOLUME", volume);
};
// endregion

// region Video
bool Settings::getFullScreen() const { return m_settings.fullScreen; };
void Settings::setFullScreen(const bool enabled)
{
    m_settings.fullScreen = enabled;
    notifyObservers("FULLSCREEN_ENABLED", enabled);
}
void Settings::setResolution(const int width, const int height)
{
    m_settings.screenWidth = width;
    m_settings.screenHeight = height;
    notifyObservers("RESOLUTION", getResolution());
}
glm::ivec2 Settings::getResolution() const
{
    return glm::ivec2{m_settings.screenWidth, m_settings.screenHeight};
};
// endregion