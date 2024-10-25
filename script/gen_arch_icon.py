from PIL import Image
from pathlib import Path

for path in Path('icons').glob("*.png"):
    print(path)
    dst = '../arch/pkg/data/' + str(path).replace('_', '/')
    im = Image.open(path)
    w, h = im.size
    _im = im.resize((128, h * 128 // w))
    im = Image.new('RGBA', (160, 160))
    im.paste(_im, (16, 16))
    im.save(dst)
