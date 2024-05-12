#include "language_string.h"
#include "config.h"

const char *TEXT(size_t index)
{
    return index < TEXT_COUNT ? gTexts[gConfig->language][index] : "Unknown";
}

const char *const LanguageString::Get() const
{
    if (_text_id != INVALID_TEXT_ENUM)
    {
        return TEXT(_text_id);
    }

    if (gConfig->language == LANGUAGE_ENGLISH)
    {
        return _string.c_str();
    }

    auto iter = gTrans.find(_string);
    if (iter == gTrans.end())
    {
        return _string.c_str();
    }
    else
    {
        return iter->second[gConfig->language - 1];
    }
};