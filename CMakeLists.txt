cmake_minimum_required(VERSION 3.12)

set(CMAKE_DEPENDS_USE_COMPILER FALSE)
set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VITASDK}/share/vita.toolchain.cmake"
    CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

option(WITH_LOG "log ouputting" OFF)

find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

set(CORES
    gpsp
    vba_next
    gambatte
    fba_lite
    fbneo
    fbalpha2012
    snes9x2002
    snes9x2005
    snes9x2010
    fceumm
    nestopia
    genesis_plus_gx
    picodrive
    mednafen_pce_fast
    mednafen_supergrafx
    mednafen_ngp
    mednafen_wswan
    pcsx_rearmed)

set(BUILD_LIST "ALL" "Arch" ${CORES})
string(REPLACE "|" " " BUILD_LIST_STR ${BUILD_LIST})
set(BUILD "Arch" CACHE STRING ${BUILD_LIST_STR})
set_property(CACHE BUILD PROPERTY STRINGS ${BUILD_LIST})

string(STRIP ${BUILD} BUILD)

if(NOT ${BUILD} IN_LIST BUILD_LIST)
    message(FATAL_ERROR "valid BUILD value: ${BUILD_LIST}")
endif()

if(BUILD STREQUAL "Arch")
    set(BUILD_SELF ON)
endif()

project("Emu4Vita++")

if(WITH_LOG)
    set(LOG_LEVEL LOG_LEVEL_DEBUG)
else()
    set(LOG_LEVEL LOG_LEVEL_INFO)
endif()

set(COMMON_CMAKE_C_FLAGS "-marm -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard \
-DVITA -D__vita__ \
-mword-relocations -fno-optimize-sibling-calls -fsingle-precision-constant \
-fomit-frame-pointer -fno-unwind-tables -fno-asynchronous-unwind-tables \
-ffast-math -ftree-vectorize -O3 \
-DLOG_LEVEL=${LOG_LEVEL}")
set(COMMON_CMAKE_CXX_FLAGS "${COMMON_CMAKE_C_FLAGS} -fno-rtti -std=gnu++17 -fno-exceptions")
set(COMMON_CMAKE_EXE_LINKER_FLAGS "-Wl,-q,-no-enum-size-warning,--as-needed") # ,--no-undefined,--as-needed,--hash-style=gnu")

set(FBNEO_CMAKE_C_FLAGS "-DLOG_LEVEL=${LOG_LEVEL} \
-mfloat-abi=hard -fsingle-precision-constant \
-ffloat-store \
-finline-limit=1200 \
-fcheck-new \
-ffast-math \
-frounding-math \
")
set(FBNEO_CMAKE_CXX_FLAGS "${FBNEO_CMAKE_C_FLAGS} -fno-rtti -fpermissive")
set(FBNEO_CMAKE_EXE_LINKER_FLAGS "-Wl,-q")

macro(SET_FLAG core)
    if(${core} STREQUAL "fbneo")
        set(CMAKE_C_FLAGS ${FBNEO_CMAKE_C_FLAGS})
        set(CMAKE_CXX_FLAGS ${FBNEO_CMAKE_CXX_FLAGS})
        set(CMAKE_EXE_LINKER_FLAGS ${FBNEO_CMAKE_EXE_LINKER_FLAGS})
    else()
        set(CMAKE_C_FLAGS ${COMMON_CMAKE_C_FLAGS})
        set(CMAKE_CXX_FLAGS ${COMMON_CMAKE_CXX_FLAGS})
        set(CMAKE_EXE_LINKER_FLAGS ${COMMON_CMAKE_EXE_LINKER_FLAGS})
    endif()
endmacro()

SET_FLAG(BUILD)

set(OUT_PATH "${CMAKE_BINARY_DIR}/out")
file(MAKE_DIRECTORY ${OUT_PATH})

add_subdirectory(deps)
add_subdirectory(share)

set(APP_VER "0.1")
set(SFO_APP_VER "00.01")

foreach(core ${CORES})
    if(BUILD STREQUAL "ALL" OR BUILD STREQUAL "Arch" OR BUILD STREQUAL ${core})
        SET_FLAG(core)
        add_subdirectory(apps/${core})
    endif()
endforeach()

if(BUILD STREQUAL "ALL" OR BUILD STREQUAL "Arch")
    set(CMAKE_C_FLAGS ${COMMON_CMAKE_C_FLAGS})
    set(CMAKE_CXX_FLAGS ${COMMON_CMAKE_CXX_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS ${COMMON_CMAKE_EXE_LINKER_FLAGS})
    add_subdirectory(arch)
endif()