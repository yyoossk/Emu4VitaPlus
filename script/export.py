#!/usr/bin/env python
from trans import Translation

trans = Translation('language.json').get_trans(index='Tag')

languages = None

header = []
header.append('#pragma once\n')
header.append('emun {')
for i, (k, v) in enumerate(trans.items()):
    if i == 0:
        attr = f'{k} = 0,'
    else:
        attr = f'{k},'
    header.append(f'    {attr:20s} // "{v["English"]}"')
    if languages is None:
        languages = list(filter(lambda x: x not in ('No.', 'Tag', 'Comments'), v.keys()))
header.append('};\n')

header.append('emun {')
for i, language in enumerate(languages):
    header.append(f'    LANGUAGE_{language.upper()},')
header.append('    LANGUAGE_COUNT,')
header.append('};\n')

for language in languages:
    header.append(f'const char *gLanguage{language}[];')


open('language_define.h', 'w').write('\n'.join(header))

source = []
for language in languages:
    source.append(f'const char *gLanguage{language}[] = {{')
    for k, v in trans.items():
        s = f'"{v[language]}",'
        source.append(f'    {s:30s} // {k}')
    source.append('};\n')
open('language_define.cpp', 'w').write('\n'.join(source))
