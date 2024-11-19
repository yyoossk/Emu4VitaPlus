from fontTools.ttLib import TTFont

code_map = {0x1F52B: 0xF52B, 0x1F579: 0xF579}

font = TTFont('promptfont.ttf')

cmap = font['cmap']
for table in cmap.tables:
    if table.format == 12:  # 常见格式
        cmap_table = table


for old, new in code_map.items():
    glyph_name = cmap_table.cmap.get(old)
    print(glyph_name)
    del cmap_table.cmap[old]
    cmap_table.cmap[new] = glyph_name

font.save('../share/pkg/assets/promptfont.ttf')
