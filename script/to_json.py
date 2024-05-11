#!/usr/bin/env python
from trans import Translation

for name in ('language', 'translation'):
    trans = Translation(f'{name}.xlsx')
    trans.save(f'{name}.json', index='Tag')
