# Emu4Vita++
This is an emulator frontend for the PlayStation Vita, based on the [Libretro API](https://github.com/libretro/libretro-common).

# Pre-Compilation Preparation
## Windows
* Install [msys2](https://www.msys2.org/) or [devkitPro](https://github.com/devkitPro/installer/releases)
#### Enter msys
* Install [vitasdk](https://vitasdk.org/)
* Install cmake, python, ccache (optional)

  ```bash
  pacman -S cmake python python-pip [ccache]
  ```

* Install [openpyxl](https://pypi.org/project/openpyxl/)

  ```bash
  pip install openpyxl
  ```

## Linux

Refer to the steps for Windows. If you're using Linux, you should be capable of figuring it out yourself.

# Compilation
```bash
mkdir build
cd build
cmake ../
make
```
Compiling a version with logs:
```bash
mkdir build-debug
cd build-debug
cmake ../ -DWITH_LOG=ON
make
```
Compiling a standalone core vpk:
```bash
mkdir build-gpsp
cd build-gpsp
cmake ../ -DBUILD=gpsp
make
```