#!/usr/bin/env python
from trans import Translation

trans = Translation('language.json')
trans.save('language.xlsx', index='Tag')
