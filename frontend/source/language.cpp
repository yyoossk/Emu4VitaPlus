#include "language.h"

Language::Language()
{
    Set(LANGUAGE_CHINESE);
}

Language::~Language()
{
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
