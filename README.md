[For English](README.en.md)

# Emu4Vita++
这是一个在 PlayStation Vita 使用的, 基于 [Libretro API](https://github.com/libretro/libretro-common) 的模拟器前端。

# 下载
[Release](https://github.com/noword/Emu4VitaPlus/releases)

[百度网盘](https://pan.baidu.com/s/1chcOOw9G1GBtlkM9K4MtRg?pwd=E4VP)

带log的版本会在 `ux0:data/EMU4VITAPLUS/[core]/Emu4Vita++.log` 中输出更多日志

# 截屏
![](screenshots/arch.jpg)
![](screenshots/browser.jpg)
![](screenshots/control.jpg)
![](screenshots/hotkey.jpg)

## [遮罩和着色器](GRAPHICS.md)


# 支持的内核
街机
  - [FinalBurn Lite](https://gitee.com/yizhigai/libretro-fba-lite)
  - [FinalBurn Alpha 2012](https://github.com/libretro/fbalpha2012.git)
  - [FinalBurn Neo](https://github.com/libretro/FBNeo.git)

NES
  - [FCEUmm](https://github.com/libretro/libretro-fceumm)
  - [Nestopia](https://github.com/libretro/nestopia)

SNES
  - [Snes9x 2002](https://github.com/libretro/snes9x2002)
  - [Snes9x 2005](https://github.com/libretro/snes9x2005)
  - [Snes9x 2010](https://github.com/libretro/snes9x2010)

MD
  - [Genesis Plus GX](https://github.com/libretro/Genesis-Plus-GX)
  - [Genesis Plus GX Wide](https://github.com/libretro/Genesis-Plus-GX-Wide.git)
  - [PicoDrive](https://github.com/libretro/picodrive)
  
GBC
  - [Gambatte](https://github.com/libretro/gambatte-libretro)

GBA
  - [gpSP](https://github.com/libretro/gpsp)
  - [VBA Next](https://github.com/libretro/vba-next)
  - [mGBA](https://github.com/libretro/mgba.git)

PCE
  - [Mednafen PCE Fast](https://github.com/libretro/beetle-pce-fast-libretro)
  - [Mednafen SuperGrafx](https://github.com/libretro/beetle-supergrafx-libretro)

PS1
  - [PCSX ReARMed](https://github.com/libretro/pcsx_rearmed)

WSC
  - [Mednafen Wonderswan](https://github.com/libretro/beetle-wswan-libretro)

NGP
  - [Mednafen NeoPop](https://github.com/libretro/beetle-ngp-libretro)

# 编译前准备
## Windows
* 安装 [msys2](https://www.msys2.org/) 或 [devkitPro](https://github.com/devkitPro/installer/releases)
#### 进入 msys
* 安装 [vitasdk](https://vitasdk.org/)
* 安装 cmake, python, ccache(可选)
  
  ```bash
  pacman -S cmake python python-pip [ccache]
  ```

* 安装 [openpyxl](https://pypi.org/project/openpyxl/), [pillow](https://pypi.org/project/pillow/)

  ```bash
  pip3 install openpyxl pillow
  ```

## Linux

参考 Windows 的步骤，都用 Linux 了，应该有能力自己捣鼓了。

# 编译
```bash
mkdir build
cd build
cmake ../
make
```
编译带日志的版本:
```bash
mkdir build-debug
cd build-debug
cmake ../ -DWITH_LOG=ON
make
```
编译独立内核的vpk:
```bash
mkdir build-gpsp
cd build-gpsp
cmake ../ -DBUILD=gpsp
make
```

# 相关脚本
## 翻译

```mermaid
graph TD
A[./to_exce.py] --> B{{编辑 language.xlsx 和 translation.xlsx}}
B --> C[./to_json.py]
```

## 生成 shaders
```bash
./compile_shaders.py  #请自行在网上寻找 psp2cgc.exe
```

# 相关路径
## 预览图
程序会在 rom 目录下的 `.previews` 目录中自动寻找同名的 `jpg` 或 `png` 图片，如果未找到，则会搜寻即时存档的截图

# 遮罩
程序会读取 ux0:/data/EMU4VITAPLUS/[内核]/overlays 下的 overlays.ini

## 即时存档
ux0:/data/EMU4VITAPLUS/[内核]/savestates/[rom]

## 存档
ux0:/data/EMU4VITAPLUS/[内核]/savefiles

## BIOS
ux0:/data/EMU4VITAPLUS/[内核]/system

请自行将对应的 BIOS 文件复制到此目录中【重要】

# 特别感谢
[一直改](https://gitee.com/yizhigai/Emu4Vita)

[KyleBing](https://github.com/KyleBing/retro-game-console-icons)

以及在本项目中用到的所有开源项目的开发者们