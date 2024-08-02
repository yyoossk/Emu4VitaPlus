#!/usr/bin/env python
import re
import sys


def extract_strings_from_file(file_path):
    string_pattern = re.compile(r'".*?"|\'.*?\'', re.DOTALL)

    strings = []
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
        content = file.read()
        matches = string_pattern.findall(content)
        for match in matches:
            strings.append(match.strip('"'))
    return strings


if __name__ == '__main__':
    for s in extract_strings_from_file(sys.argv[1]):
        print(s)
