cmake_minimum_required(VERSION 3.12)

set(CMAKE_DEPENDS_USE_COMPILER FALSE)
set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VITASDK}/share/vita.toolchain.cmake"
    CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

option(WITH_LOG "log ouputting" OFF)
set(BUILD_LIST "ALL" "Arch" "gpsp" "fba_lite" "snes9x2002" "snes9x2005" "snes9x2010 ")
set(BUILD "ALL" CACHE STRING "ALL(default)|Arch|gpsp|fba_lite|snes9x2002|snes9x2005|snes9x2010")
set_property(CACHE BUILD PROPERTY STRINGS ${BUILD_LIST})

string(STRIP ${BUILD} BUILD)

if(NOT ${BUILD} IN_LIST BUILD_LIST)
    message(FATAL_ERROR "valid BUILD value: ${BUILD_LIST}")
endif()

project(" Emu4Vita++ ")

set(CMAKE_C_FLAGS " -marm -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard \
-DVITA -D__vita__ \
-mword-relocations -fno-optimize-sibling-calls \
-fomit-frame-pointer -fno-unwind-tables -fno-asynchronous-unwind-tables \
-ffast-math -ftree-vectorize -O3 ")

if(WITH_LOG)
    set(LOG_LEVEL LOG_LEVEL_DEBUG)
else()
    set(LOG_LEVEL LOG_LEVEL_INFO)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOG_LEVEL=${LOG_LEVEL}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -std=c++17")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-q") #,--no-undefined,--as-needed,--hash-style=gnu")

set(OUT_PATH "${CMAKE_BINARY_DIR}/out")
file(MAKE_DIRECTORY ${OUT_PATH})

add_subdirectory(deps)
add_subdirectory(share)

set(APP_VER "0.1")
set(SFO_APP_VER "00.01")

if(BUILD STREQUAL "ALL" OR BUILD STREQUAL "gpsp")
    add_subdirectory(apps/gpsp)
endif()

if(BUILD STREQUAL "ALL" OR BUILD STREQUAL "fba_lite")
    add_subdirectory(apps/fba_lite)
endif()

if(BUILD STREQUAL "ALL" OR BUILD STREQUAL "snes9x2002")
    add_subdirectory(apps/snes9x2002)
endif()

if(BUILD STREQUAL "ALL" OR BUILD STREQUAL "snes9x2005")
    add_subdirectory(apps/snes9x2005)
endif()

if(BUILD STREQUAL "ALL" OR BUILD STREQUAL "snes9x2010")
    add_subdirectory(apps/snes9x2010)
endif()

if(BUILD STREQUAL "ALL" OR BUILD STREQUAL "Arch")
    add_subdirectory(arch)
endif()