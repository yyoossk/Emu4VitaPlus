#include "string.h"
#include "language.h"

Language::Language()
{
    Set(LANGUAGE_CHINESE);
}

Language::~Language()
{
}

void Language::Set(const char *lang_str)
{
    for (size_t i = 0; i < LANGUAGE_COUNT; i++)
    {
        if (strcmp(lang_str, gLanguageNames[i]) == 0)
        {
            return Set(LANGUAGE(i));
        }
    }
    return Set(LANGUAGE::LANGUAGE_ENGLISH);
}

void Language::Set(LANGUAGE lang)
{
    _lang = lang;
}

LANGUAGE Language::Get()
{
    return _lang;
}

const char *Language::GetName()
{
    return gLanguageNames[_lang];
}

const char *Language::Get(size_t index)
{
    return index < TEXT_COUNT ? gTexts[_lang][index] : "Unknown";
}
