from cairosvg import svg2png
from io import BytesIO
from PIL import Image, ImageDraw, ImageFont
from colorthief import ColorThief
import shutil
from cores import CORES


# svg from https://gitlab.com/recalbox/recalbox-themes


NOT_DARKEST = {'DOS': -1, 'SNES': -1}

WIDTH, HEIGHT = 840, 500
CONSOLE_WIDTH, CONSOLE_HEIGHT = 480, 400
CONSOLE_IMAGE_WIDTH = int(CONSOLE_WIDTH * 0.8)
LOGO_WIDTH, LOGO_HEIGHT = 480, 100
STRIP_WIDTH = 20
TEXT_X = CONSOLE_WIDTH + STRIP_WIDTH * 4
TEXT_Y = HEIGHT - 210
TEXT_W = WIDTH - TEXT_X
TEXT_H = 210 // 2
FONT = ImageFont.truetype('AlibabaPuHuiTi-2-75-SemiBold.ttf', 32)
SMALL_FONT = ImageFont.truetype('AlibabaPuHuiTi-2-65-Medium.ttf', 24)
TEXT = 'Emu4Vita++'
TEXT_SMALL = 'By noword'


def create_gradient_image(width, height, color1, color2):
    image = Image.new("RGBA", (width, height))
    pixels = image.load()

    # 提取颜色通道
    r1, g1, b1 = color1
    r2, g2, b2 = color2

    for y in range(height):
        for x in range(width):
            # 计算位置权重，从右上到左下
            weight_x = x / (width - 1)
            weight_y = 1 - y / (height - 1)
            weight = (weight_x + weight_y) / 2

            # 插值计算颜色
            r = int(r1 * weight + r2 * (1 - weight))
            g = int(g1 * weight + g2 * (1 - weight))
            b = int(b1 * weight + b2 * (1 - weight))

            # 设置像素值，添加固定 Alpha 通道
            pixels[x, y] = (r, g, b, 0xFF)

    return image


def calculate_complementary_color(color):
    r, g, b = color
    return (0xFF - r, 0xFF - g, 0xFF - b)


def get_luminance(color):
    r, g, b = color
    return (0.299 * r + 0.587 * g + 0.114 * b) / 255


def do(console_name, cores):
    print(console_name)
    out = BytesIO()
    svg2png(url=f'svgs/{console_name}/console.svg', write_to=out, output_height=350)
    console = Image.open(out).convert('RGBA')

    out = BytesIO()
    svg2png(url=f'svgs/{console_name}/logo.svg', write_to=out, output_height=50)
    logo = Image.open(out).convert('RGBA')

    im = Image.new('RGBA', (WIDTH, HEIGHT))

    w, h = console.size
    if w > CONSOLE_IMAGE_WIDTH:
        console = console.resize((CONSOLE_IMAGE_WIDTH, h * CONSOLE_IMAGE_WIDTH // w))
        w, h = console.size
    im.paste(console, ((CONSOLE_WIDTH - w) // 2, (CONSOLE_HEIGHT - h) // 2), console)
    w, h = logo.size
    im.paste(logo, (((LOGO_WIDTH - w) // 2, CONSOLE_HEIGHT + (LOGO_HEIGHT - h) // 2)), logo)

    out = BytesIO()
    im.save(out, format='png')
    color_thief = ColorThief(out)
    colors = color_thief.get_palette(4, 1)
    draw = ImageDraw.Draw(im)
    for i, color in enumerate(colors):
        x = CONSOLE_WIDTH + i * STRIP_WIDTH
        draw.rectangle((x, 0, x + STRIP_WIDTH, HEIGHT), fill=color)

    colors.sort(key=lambda x: get_luminance(x))

    if console_name in NOT_DARKEST:
        dark_color = colors[NOT_DARKEST[console_name]]
    else:
        dark_color = colors[0]

    left, top, right, buttom = draw.textbbox((0, 0), TEXT, font=FONT)
    w = right - left
    h = buttom - top
    x = TEXT_X + (TEXT_W - w) // 2
    y = TEXT_Y + (TEXT_H - h) // 2
    draw.text((x, y), TEXT, font=FONT, fill=dark_color)

    w = draw.textlength(TEXT_SMALL, font=SMALL_FONT)
    x = TEXT_X + (TEXT_W - w) // 2
    draw.text((x, y + h * 2), TEXT_SMALL, font=SMALL_FONT, fill=dark_color)

    bg = create_gradient_image(
        WIDTH,
        HEIGHT,
        calculate_complementary_color(colors[0]),
        calculate_complementary_color(colors[-1]),
    )
    bg.paste(im, mask=im)
    bg = bg.convert('RGB')
    bg.save(f'{console_name}.png')
    for core in cores:
        name = f'../apps/{core}/pkg/sce_sys/livearea/contents/bg.png'
        shutil.copy(f'{console_name}.png', name)


for console, cores in CORES.items():
    do(console, cores)