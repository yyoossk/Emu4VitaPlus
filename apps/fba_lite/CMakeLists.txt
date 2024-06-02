cmake_minimum_required(VERSION 3.10)

if(TARGET fba_lite_build)
  return()
endif()

set(CMAKE_TOOLCHAIN_FILE
  "$ENV{VITASDK}/share/vita.toolchain.cmake"
  CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

set(APP_NAME "FbaLite4vita")
set(APP_DIR_NAME "fba_lite")
set(CORE_NAME "fba_lite")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/libretro-fba-lite)
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/makefile.libretro)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_NAME})")
set(SFO_TITLE_ID "FBALITE4V")
set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)

set(CORE_ARGS platform=vita EXTERNAL_ZLIB=1)

set(CORE_SOFTWARE, "Arcade (CPS1 2 3, NEO GEO, PGM)")

file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

project("fba_lite")

add_custom_target(
  fba_lite_build ALL
  COMMAND alias gcc="${CMAKE_C_COMPILER}" && make -f ${CORE_MAKEFILE_NAME} -j ${CORE_ARGS}
  WORKING_DIRECTORY ${CORE_DIR})

add_library(fba_lite STATIC IMPORTED)
set_property(
  TARGET fba_lite
  APPEND
  PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)

set(LIBRETRO_LIBRARY ${CORE_DIR}/fba_lite_libretro_vita.a)
set_target_properties(fba_lite PROPERTIES IMPORTED_LOCATION_NOCONFIG
  "${LIBRETRO_LIBRARY}")
add_dependencies(fba_lite fba_lite_build)

add_compile_definitions(FBA_LITE_BUILD ARC_BUILD FBA_BUILD)
add_subdirectory(../../frontend ${CMAKE_CURRENT_BINARY_DIR}/frontend)