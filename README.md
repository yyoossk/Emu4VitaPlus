<!-- [English](README.en.md) -->

# 编译前准备
## Windows
* 安装 [msys2](https://www.msys2.org/) 或 [devkitPro](https://github.com/devkitPro/installer/releases)
* 安装 [vitasdk](https://vitasdk.org/)
* 安装 [openpyxl](https://pypi.org/project/openpyxl/)

  `pip install openpyxl`

* 安装 cmake 
  
  `pacman -S cmake`

## Linux

参考 Windows 的步骤，都用 Linux 了，应该有能力自己捣鼓了。

# 编译
```bash
mkdir build
cd build
cmake ../
make
```
编译带日志的版本
```bash
cmake ../ -DWITH_LOG=ON
```