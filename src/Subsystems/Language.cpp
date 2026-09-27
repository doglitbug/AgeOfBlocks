#include "Language.h"

Language::Language()
{
    //TODO Load language csv
}

void Language::onNotify(const std::string& message, MyType newValue)
{
    if (message == "LANGUAGE")
    {
        mCurrentLanguage = std::get<std::string>(newValue);
    }
}

std::string Language::get(const std::string& key)
{
    if (const auto it = mStringTable.find(key); it != mStringTable.end()) {
        return it->second;
    }
    return "MISSING_STRING: " + key;
}

void Language::loadLanguage()
{
    mStringTable.clear();
    
}
