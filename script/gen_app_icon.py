from PIL import Image
from pathlib import Path
from cores import CORES


for console, cores in CORES.items():
    im = Image.open(f'icons/{console}_icon0.png')
    w, h = im.size
    _im = im.resize((110, h * 110 // w))
    im = Image.new('RGB', (128, 128), '#23446d')
    im.paste(_im, (10, 18), _im)
    im = im.quantize()
    for name in cores:
        print(name)
        im.save(f'../apps/{name}/pkg/sce_sys/icon0.png')
