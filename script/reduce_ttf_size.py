from fontTools.ttLib import TTFont
from fontTools.subset import Subsetter, Options

# 加载TTF字体文件
input_font_path = "FZLantingHei.ttf"
output_font_path = "../share/pkg/assets/FZLantingHei.ttf"
font = TTFont(input_font_path)

# 创建子集化选项，删除Hinting信息
options = Options()
options.hinting = False  # 禁用Hinting数据
options.desubroutinize = True  # 去除复杂字形中的子程序引用
options.flavor = None  # 不进行额外压缩（可选）
options.drop_tables += ['DSIG']  # 删除特定表格，比如数字签名表‘DSIG’（通常无用）

# 创建子集化器
subsetter = Subsetter(options=options)

# 因为不减少字符数量，所以将所有字形加载到子集化器中
# subsetter.populate(text="需要的字符集")
subsetter.populate(glyphs=font.getGlyphOrder())
subsetter.subset(font)

# 进一步优化表格数据
for table in ['FFTM', 'GDEF', 'LTSH', 'PCLT', 'VDMX', 'hdmx', 'kern', 'morx', 'opbd']:
    if table in font:
        del font[table]  # 删除一些冗余表格，如果存在的话

# 保存优化后的字体文件
font.save(output_font_path)

print(f"字体优化完成，保存到：{output_font_path}")
