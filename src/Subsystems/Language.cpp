#include "Language.h"

#include "App.h"

Language::Language()
{
    m_currentLanguage = App::get()->getSettings()->getLanguage();
    loadLanguage();
}

void Language::onNotify(const std::string& message, const MyType newValue)
{
    if (message == "LANGUAGE")
    {
        m_currentLanguage = std::get<std::string>(newValue);
    }
}

std::string Language::get(const std::string& key)
{
    if (const auto it = m_stringTable.find(key); it != m_stringTable.end())
    {
        return it->second;
    }
    return "MISSING_STRING: " + key;
}

void Language::loadLanguage()
{
    m_stringTable.clear();
    //TODO Mock data is here, go load from file!
    m_stringTable["LANGUAGE"] = "English";

    m_stringTable["MENU_NAME"] = "Age of Blocks";
    m_stringTable["MENU_SINGLEPLAYER"] = "New game";
    m_stringTable["MENU_MULTIPLAYER"] = "Multiplayer";
    m_stringTable["MENU_SETTINGS"] = "Settings";
    m_stringTable["MENU_CREDITS"] = "Credits";
    m_stringTable["MENU_QUIT"] = "Quit";
}
