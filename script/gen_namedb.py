import json
import lz4.block
from zlib import crc32
from io import BytesIO
from struct import pack
from cores import CORES
import shutil

LANGS = (
    ('English', 'en'),
    ('Chinese', 'cn'),
)


def str_crc32(s):
    return crc32(str(s).encode('utf-8')) & 0xFFFFFFFF


def gen_db(infos, db_name):
    map_io = BytesIO()
    name_io = BytesIO()
    map_io.write(pack('I', len(infos)))
    for key, name in infos.items():
        map_io.write(pack('II', key, name_io.tell()))
        name_io.write(name.encode('utf-8') + b'\x00')

    buf = map_io.getvalue() + pack('I', name_io.tell()) + name_io.getvalue()
    zbuf = lz4.block.compress(buf, mode='high_compression', store_size=False, compression=12)

    open(db_name + '.bin', 'wb').write(buf)

    with open(db_name + '.zdb', 'wb') as fp:
        fp.write(pack('II', len(buf), len(zbuf)))
        fp.write(zbuf)


names = json.load(open('rom_db/arcade.names.json', encoding='utf-8'))
for lang_name, lang_code in LANGS:
    infos = {}
    for n in names:
        infos[str_crc32(n['File'])] = n[lang_name]
    db_name = f'names.{lang_code}'
    gen_db(infos, db_name)
    db_name += ".zdb"
    shutil.copy(db_name, '../arch/pkg/data/ARC')
    for core in CORES['ARC']:
        shutil.copy(db_name, f'../apps/{core}/pkg/assets')
