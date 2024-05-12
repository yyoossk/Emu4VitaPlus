[English](README.en.md)

# 编译前准备
* 如果是Windows，先安装 [msys2](https://www.msys2.org/) 或 [devkitPro](https://github.com/devkitPro/installer/releases)
* 安装 [vitasdk](https://vitasdk.org/)
* 安装 [openpyxl](https://pypi.org/project/openpyxl/)

  `pip install openpyxl`

* 安装 cmake 
  
  `pacman -S cmake`



# 编译
```
mkdir build
cd build
cmake ../
make
```
编译带日志的版本
```
cmake ../ -DWITH_LOG=ON
```