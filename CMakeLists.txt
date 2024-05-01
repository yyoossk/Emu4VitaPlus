cmake_minimum_required(VERSION 3.12)

set(CMAKE_DEPENDS_USE_COMPILER FALSE)

set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VITASDK}/share/vita.toolchain.cmake"
    CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

project("Emu4Vita++")

set(CMAKE_C_FLAGS
    "-marm -mcpu=cortex-a9 -mfloat-abi=hard -DVITA -D__vita__ \
    -mword-relocations -fno-optimize-sibling-calls \
    -fomit-frame-pointer -fno-unwind-tables -fno-asynchronous-unwind-tables \
    -ffast-math -ftree-vectorize -O3")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-DDEBUG -g")
set(LDFLAGS "-Wl,-q")

add_subdirectory(deps)
add_subdirectory(apps/gpsp)
