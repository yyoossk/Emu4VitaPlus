#pragma once
#include <string>
#include "global.h"

class LanguageString
{
public:
    LanguageString(TEXT_ENUM text_id) : _text_id(text_id){};
    LanguageString(const char *str) : _string(str){};
    LanguageString(size_t id) : _text_id(TEXT_ENUM(id)){};

    const char *const Get()
    {
        return _string.size() > 0 ? _string.c_str() : TEXT(_text_id);
    };

private:
    TEXT_ENUM _text_id;
    std::string _string;
};