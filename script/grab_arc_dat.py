import xml.etree.ElementTree as ET
from pathlib import Path
from zlib import crc32
from io import BytesIO
from struct import pack


def AlignUp(offset, align):
    align -= 1
    return (offset + align) & ~align


def WritePadding(io, align, value=b'\x00'):
    align -= 1
    offset = io.tell()
    size = ((offset + align) & ~align) - offset
    io.write(value * size)


def Grab(path):
    data_io = BytesIO()
    name_offsets = {}
    names = set()
    roms = {}

    for name in Path(path).glob('*.dat'):
        tree = ET.parse(name)
        root = tree.getroot()
        for game in root.iter('game'):
            name = game.attrib['name']
            names.add(crc32(name.encode('utf-8')) & 0xFFFFFFFF)
            if name not in name_offsets:
                name_offsets[name] = data_io.tell()
                data_io.write(name.encode('utf-8'))
                data_io.write(b'\x00')

            for rom in game.iter('rom'):
                crc = rom.get('crc')
                if crc:
                    if crc in roms:
                        roms[crc].add(name_offsets[name])
                    else:
                        roms[crc] = {name_offsets[name]}

    WritePadding(data_io, 4)
    return names, roms, data_io.getvalue()


names, roms, name_data = Grab('../cores/libretro-fbneo/dats/')
names_crcs = ',\n'.join([f'0x{n:08x}' for n in names])
data_array = ','.join([hex(d) for d in name_data])

with open('roms.txt', 'w') as fp:
    for crc, ns in roms.items():
        fp.write(f'{int(crc, 16):08x} : ')
        fp.write(', '.join([f'{n:08x}' for n in ns]))
        fp.write('\n')

with open('arcade_dat.bin', 'wb') as fp:
    fp.write(pack('I', len(name_data)))
    fp.write(name_data)
    fp.write(pack('I', len(names)))
    fp.write(pack(f'{len(names)}I', *names))
    fp.write(pack('I', len(roms)))
    for crc, names in roms.items():
        fp.write(pack('II', len(names), int(crc, 16)))
        fp.write(pack(f'{len(names)}I', *names))
