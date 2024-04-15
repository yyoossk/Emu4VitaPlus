cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

project("Emu4Vita++")

vita_create_self(eboot.bin ${CMAKE_PROJECT_NAME}.elf)
add_subdirectory(apps/gpsp)