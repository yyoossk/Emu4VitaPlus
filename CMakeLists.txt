cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

project("Emu4Vita++")

set(CMAKE_C_FLAGS "-marm -mcpu=cortex-a9 -mfloat-abi=hard -DVITA -D__vita__ \
    -mword-relocations -fno-optimize-sibling-calls \
    -fomit-frame-pointer -fno-unwind-tables -fno-asynchronous-unwind-tables \
    -ffast-math -ftree-vectorize -O3")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -std=c++17")

add_subdirectory(deps)

# add_subdirectory(apps/gpsp)
file(GLOB SRC
    frontend/source/*.cpp
    frontend/source/ui/*.cpp
    frontend/source/audio/*.cpp)

set(INC frontend/source
    frontend/source/ui
    frontend/source/audio
    deps/libretro-common/include
    deps/tomlplusplus/include
)

add_executable(${CMAKE_PROJECT_NAME}.elf ${SRC})
target_include_directories(${CMAKE_PROJECT_NAME}.elf PRIVATE ${INC})
target_link_libraries(${CMAKE_PROJECT_NAME}.elf retro_comm)

vita_create_self(eboot.bin ${CMAKE_PROJECT_NAME}.elf)
