import json
from pathlib import Path
from zlib import crc32
from io import BytesIO
from struct import pack
import lz4.block


def get_info(path, key='crc'):
    infos = {}
    for line in open(path):
        d = json.loads(line)

        if key == 'crc':
            crc = int(d['crc'], 16)
        elif key == 'rom_name':
            crc = crc32(Path(d['rom_name']).stem) & 0xFFFFFFFF
        else:
            raise TypeError
        if 'name' not in d:
            print(d)
            continue
        name = d['name']
        # if crc in infos:
        #     print(f'Duplicate key: {crc:08x} {infos[crc]} / {name}')
        infos[crc] = name


def get_arc_info(path):
    infos = {}
    for line in open(path):
        d = json.loads(line)
        key = crc32(str(Path(d['rom_name'])).encode('ascii')) & 0xFFFFFFFF
        if 'name' not in d:
            print(d)
            continue
        infos[key] = d['name']
    return infos


def gen_db(infos, path):
    map_io = BytesIO()
    name_io = BytesIO()
    map_io.write(pack('I', len(infos)))
    for key, name in infos.items():
        map_io.write(pack('II', key, name_io.tell()))
        name_io.write(name.encode('utf-8') + b'\x00')

    buf = map_io.getvalue() + pack('I', name_io.tell()) + name_io.getvalue()
    zbuf = lz4.block.compress(buf, mode='high_compression', store_size=False)

    open('1.bin', 'wb').write(buf)

    with open(path, 'wb') as fp:
        fp.write(pack('II', len(buf), len(zbuf)))
        fp.write(zbuf)


if __name__ == '__main__':
    import sys

    # infos = get_info('roms/Nintendo - Nintendo Entertainment System.json')
    # print(infos)
    infos = {}
    for name in (
        'FBNeo - Arcade Games.json',
        'MAME 2000.json',
        'MAME 2003-Plus.json',
        'MAME 2003.json',
        'MAME 2010.json',
        'MAME 2015.json',
        'MAME 2016.json',
        'MAME.json',
    ):
        infos.update(get_arc_info('roms/' + name))

    gen_db(infos, 'names.en.zdb')
