#!/usr/bin/env python
from trans import Translation

trans = Translation('language.json').get_trans(index='Tag')

# Generate language_define.h
languages = None

TEXT_ENUM = []
LANGUAGE_ENUM = []

for i, (k, v) in enumerate(trans.items()):
    attr = f'{k},'
    TEXT_ENUM.append(f'    {attr:20s} // "{v["English"]}"')
    if languages is None:
        languages = list(filter(lambda x: x not in ('No.', 'Tag', 'Comments'), v.keys()))
TEXT_ENUM.append('    TEXT_COUNT,')

for i, language in enumerate(languages):
    LANGUAGE_ENUM.append(f'    LANGUAGE_{language.upper()},')
LANGUAGE_ENUM.append('    LANGUAGE_COUNT,')

TEXT_ENUM = '\n'.join(TEXT_ENUM)
LANGUAGE_ENUM = '\n'.join(LANGUAGE_ENUM)

with open('language_define.h', 'w') as fp:
    fp.write(
        f'''#pragma once

enum TEXT_ENUM{{
{TEXT_ENUM}
}};

enum LANGUAGE{{
{LANGUAGE_ENUM}
}};

extern const char *gTexts[][TEXT_ENUM::TEXT_COUNT];
extern const char *gLanguageNames[];
'''
    )


# Generate language_define.cpp
TEXTS = []
for language in languages:
    T = []
    for k, v in trans.items():
        s = f'"{v[language]}",'
        T.append(f'    {s:30s} // {k}')
    T = '\n'.join(T)
    TEXTS.append(
        f'''// {language}
{{
{T}
}},
'''
    )
TEXTS = '\n'.join(TEXTS)


NAMES = []
for language in languages:
    NAMES.append(f'    "{language}", ')
NAMES = '\n'.join(NAMES)

with open('language_define.cpp', 'w') as fp:
    fp.write(
        f'''#include "language_define.h"

const char *gTexts[][TEXT_ENUM::TEXT_COUNT] = {{
{TEXTS}
}};

const char *gLanguageNames[] = {{
{NAMES}
}};'''
    )
