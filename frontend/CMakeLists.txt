cmake_minimum_required(VERSION 3.12)

include_directories(
    source
    source/ui
    source/audio
    source/emu
    ../share/source
    ../deps/libretro-common/include
    ../deps/libvita2d/libvita2d/include
    ../deps/libvita2d_ext/libvita2d_ext/include
    ../deps/simpleini
    ../deps/minizip-ng
    ../deps/7-Zip/C)

file(GLOB SRC
    source/*.c
    source/*.cpp
    source/ui/*.cpp
    source/emu/*.cpp
    source/audio/*.cpp)

add_library(frontend OBJECT ${SRC})
