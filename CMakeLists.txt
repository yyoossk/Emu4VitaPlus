cmake_minimum_required(VERSION 3.12)

set(CMAKE_DEPENDS_USE_COMPILER FALSE)
set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VITASDK}/share/vita.toolchain.cmake"
    CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

option(WITH_LOG "log ouputting" OFF)

project("Emu4Vita++")

set(CMAKE_C_FLAGS
    "-marm -mcpu=cortex-a9 -mfloat-abi=hard -DVITA -D__vita__ \
    -mword-relocations -fno-optimize-sibling-calls \
    -fomit-frame-pointer -fno-unwind-tables -fno-asynchronous-unwind-tables \
    -ffast-math -ftree-vectorize -O3")

if(WITH_LOG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOG_LEVEL=LOG_LEVEL_DEBUG")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-DDEBUG -g")
set(LDFLAGS "-Wl,-q")

set(OUT_PATH "${CMAKE_BINARY_DIR}/out")
file(MAKE_DIRECTORY ${OUT_PATH})

add_subdirectory(deps)
add_subdirectory(apps/gpsp)
