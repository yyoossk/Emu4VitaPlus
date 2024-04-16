cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

add_custom_target(build_libretro ALL
    COMMAND make -f Makefile.libretro_comm
)

add_library(libretro_comm STATIC IMPORTED)
set_target_properties(build_libretro
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/deps/libretro_comm/libretro_comm.a # Make sure to use absolute path here
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/deps/libretro_comm/
    #INTERFACE_COMPILE_DEFINITIONS "USING_THELIB;THELIB_STATIC"
)
add_dependencies(libretro_comm build_libretro)