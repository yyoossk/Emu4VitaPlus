#pragma once
#include <string>
#include "language_define.h"
#include "config.h"

#define INVALID_TEXT_ENUM TEXT_ENUM(-1)

#define TEXT(I) ((I) < TEXT_COUNT ? gTexts[gConfig->language][I] : "Unknown")

class LanguageString
{
public:
    LanguageString(TEXT_ENUM text_id) : _text_id(text_id){};
    LanguageString(const char *str) : _string(str), _text_id(INVALID_TEXT_ENUM){};
    LanguageString(size_t id) : _text_id(TEXT_ENUM(id)){};
    LanguageString(const LanguageString &ls) : _text_id(ls._text_id), _string(ls._string){};

    const char *const Get() const
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

private:
    TEXT_ENUM _text_id;
    std::string _string;
};