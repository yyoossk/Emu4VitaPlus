[For English](README.en.md)

# Emu4Vita++
这是一个在 PlayStation Vita 使用的, 基于 [Libretro API](https://github.com/libretro/libretro-common) 的模拟器前端。

# 编译前准备
## Windows
* 安装 [msys2](https://www.msys2.org/) 或 [devkitPro](https://github.com/devkitPro/installer/releases)
#### 进入 msys
* 安装 [vitasdk](https://vitasdk.org/)
* 安装 cmake, python, ccache(可选)
  
  `pacman -S cmake python python-pip [ccache]`

* 安装 [openpyxl](https://pypi.org/project/openpyxl/)

  `pip install openpyxl`

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
[跳转](script/README.md)