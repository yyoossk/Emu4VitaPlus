from PIL import Image
from pathlib import Path

JOBS = {
    'ARC': ('fba_lite', 'fbalpha2012', 'fbneo'),
    'GBA': ('gpsp', 'mgba', 'vba_next'),
    'GBC': ('gambatte',),
    'MD': ('genesis_plus_gx', 'genesis_plus_gx_wide', 'picodrive'),
    'NES': ('fceumm', 'nestopia'),
    'NGP': ('mednafen_ngp',),
    'PCE': ('mednafen_pce_fast', 'mednafen_supergrafx'),
    'PS1': ('pcsx_rearmed',),
    'SNES': ('snes9x2002', 'snes9x2005', 'snes9x2010'),
    'WSC': ('mednafen_wswan',),
    # 'N64': ('mupen64plus',),
}

for job, names in JOBS.items():
    im = Image.open(f'icons/{job}_icon0.png')
    w, h = im.size
    _im = im.resize((110, h * 110 // w))
    im = Image.new('RGBA', (128, 128))
    im.paste(_im, (10, 18))
    im = im.quantize()
    for name in names:
        print(name)
        im.save(f'../apps/{name}/pkg/sce_sys/icon0.png')
