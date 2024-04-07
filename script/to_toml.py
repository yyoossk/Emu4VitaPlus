#!/usr/bin/env python
from trans import Translation
import toml

trans = Translation('language.json').get_trans(index='Tag')

for v in trans.values():
    v.pop('No.')
    v.pop('Tag')
    v.pop('Comments')

toml.dump(trans, open('language.toml', 'w', encoding='utf-8'))
