#pragma once
#include <cstddef>
#include "language_define.h"

class Language
{
public:
    Language();
    virtual ~Language();

    const char *Get(size_t index);

private:
    const char **_texts;
};