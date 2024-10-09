import xml.etree.ElementTree as ET
from pathlib import Path


def Grab(path):
    names = set()
    roms = {}

    for name in Path(path).glob('*.dat'):
        tree = ET.parse(name)
        root = tree.getroot()
        for game in root.iter('game'):
            name = game.attrib['name']
            names.add(name)
            for rom in game.iter('rom'):
                crc = rom.get('crc')
                if crc:
                    if crc in roms:
                        roms[crc].add(name)
                    else:
                        roms[crc] = {name}

    return names, roms


names, roms = Grab('../cores/libretro-fbneo/dats/')
names_str = ',\n'.join([f'"{n}"' for n in names])

with open('arc_dat.cpp', 'w') as fp:
    fp.write(
        '''#include "arc_dat.h"

std::unordered_set<std::string> gArcNames = {
#ifdef ARC_BUILD
'''
    )
    fp.write(names_str)
    fp.write(
        '''
#endif
};

std::unordered_map<uint32_t, std::unordered_set<std::string>> gArcRoms = {
#ifdef ARC_BUILD
'''
    )

    for crc, names in roms.items():
        names_str = ', '.join([f'"{n}"' for n in names])
        fp.write(f'{{0x{crc}, {{{names_str}}},\n')
    fp.write(
        '''#endif
};'''
    )
