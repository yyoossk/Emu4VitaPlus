#!/usr/bin/env python
from trans import Translation

trans = Translation('language.xlsx')
trans.save('language.json', index='Tag')
