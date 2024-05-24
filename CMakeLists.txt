cmake_minimum_required(VERSION 3.12)

set(CMAKE_DEPENDS_USE_COMPILER FALSE)
set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VITASDK}/share/vita.toolchain.cmake"
    CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

option(WITH_LOG "log ouputting" OFF)
set(BUILD "ALL" CACHE STRING "ALL(default)|gpsp|fba_lite|snes9x2005")

project("Emu4Vita++")

set(CMAKE_C_FLAGS "-marm -mcpu=cortex-a9 -mfloat-abi=hard -DVITA -D__vita__ \
-mword-relocations -fno-optimize-sibling-calls \
-fomit-frame-pointer -fno-unwind-tables -fno-asynchronous-unwind-tables \
-ffast-math -ftree-vectorize -O3")

# set(CMAKE_C_FLAGS "-march=armv7-a -mfpu=neon -mfloat-abi=hard -mword-relocations -fno-optimize-sibling-calls -O2")
if(WITH_LOG)
    set(LOG_LEVEL LOG_LEVEL_DEBUG)
else()
    set(LOG_LEVEL LOG_LEVEL_INFO)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOG_LEVEL=${LOG_LEVEL}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -std=c++17")

set(LDFLAGS "-Wl,-q")

set(OUT_PATH "${CMAKE_BINARY_DIR}/out")
file(MAKE_DIRECTORY ${OUT_PATH})

add_subdirectory(deps)

if(BUILD STREQUAL "ALL" OR BUILD STREQUAL "gpsp")
    add_subdirectory(apps/gpsp)
endif()

if(BUILD STREQUAL "ALL" OR BUILD STREQUAL "fba_lite")
    add_subdirectory(apps/fba_lite)
endif()

if(BUILD STREQUAL "ALL" OR BUILD STREQUAL "snes9x2005")
    add_subdirectory(apps/snes9x2005)
endif()