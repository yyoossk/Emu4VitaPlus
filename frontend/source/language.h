#pragma once
#include <cstddef>
#include "language_define.h"

class Language
{
public:
    Language();
    virtual ~Language();

    void Set(const char *lang_str);
    void Set(LANGUAGE lang);
    LANGUAGE Get();
    const char *GetName();
    const char *Get(size_t index);

private:
    LANGUAGE _lang;
};