from PIL import Image
from pathlib import Path

ICON_SIZE = 128
IMGAE_SIZE = 158
PASTE_POS = (IMGAE_SIZE - ICON_SIZE) // 2

for path in Path('icons').glob("*.png"):
    print(path)
    dst = '../arch/pkg/data/' + path.name.replace('_', '/')
    im = Image.open(path)
    w, h = im.size
    _im = im.resize((ICON_SIZE, h * ICON_SIZE // w))
    im = Image.new('RGBA', (IMGAE_SIZE, IMGAE_SIZE))
    im.paste(_im, (PASTE_POS, PASTE_POS))
    im.save(dst)
