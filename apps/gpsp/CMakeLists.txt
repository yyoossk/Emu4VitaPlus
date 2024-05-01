cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VITASDK}/share/vita.toolchain.cmake"
    CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(APP_NAME "gpSP4vita")
set(APP_DIR_NAME "gpSP")
set(CORE_FILE_NAME "gpSP")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/${CORE_FILE_NAME})
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME Makefile)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_FILE_NAME})")
set(SFO_TITLE_ID "GPSP4VITA")
set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)

set(CORE_ARGS platform=vita EXTERNAL_ZLIB=1)

set(CORE_SOFTWARE, "Nintendo - Game Boy Advance")

file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")
foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach(PKG_FILES)

project("gpSP")

add_custom_target(
  gpSP_build ALL
  COMMAND make -f ${CORE_MAKEFILE_NAME} ${CORE_ARGS}
  WORKING_DIRECTORY ${CORE_DIR})

add_library(gpSP STATIC IMPORTED)
set_property(
  TARGET gpSP
  APPEND
  PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)

set(LIBRETRO_LIBRARY ${CORE_DIR}/gpsp_libretro_vita.a)
set_target_properties(gpSP PROPERTIES IMPORTED_LOCATION_NOCONFIG
                                      "${LIBRETRO_LIBRARY}")
add_dependencies(gpSP gpSP_build)

add_subdirectory(../../frontend ${CMAKE_CURRENT_BINARY_DIR}/frontend)
