#include "language.h"

Language::Language()
{
    _texts = gTexts[LANGUAGE_CHINESE];
}

Language::~Language()
{
}

const char *Language::Get(size_t index)
{
    return index < TEXT_COUNT ? _texts[index] : "Unknown";
}
