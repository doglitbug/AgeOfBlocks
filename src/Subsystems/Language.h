#pragma once
#include <string>

#include "Observers.h"

class Language : public IObserver
{
public:
    Language();
    ~Language() override = default;

    void onNotify(const std::string& message, MyType newValue) override;
    /**
     * Return a string in the current language for the provided key
     * @param key eg MENU_WELCOME
     * @return eg Welcome
     */
    std::string get(const std::string& key);

    // TODO Variadic template for formatting dynamic tokens safely!
    // eg You have {1} food left may have a different order in other languages
private:
    /**
     * Populate mStringTable with the current language's strings
     */
    void loadLanguage();
    std::string mCurrentLanguage;
    std::unordered_map<std::string, std::string> mStringTable;
};
