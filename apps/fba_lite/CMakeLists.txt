cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE
  "$ENV{VITASDK}/share/vita.toolchain.cmake"
  CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

set(APP_NAME "FbaLite4vita")
set(APP_DIR_NAME "fba_lite")
set(CORE_FILE_NAME "fba_lite")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/libretro-fba-lite)
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME makefile.libretro)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_FILE_NAME})")
set(SFO_TITLE_ID "FBAL4VITA")
set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)

set(CORE_ARGS )

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
  COMMAND make -f ${CORE_MAKEFILE_NAME} ${CORE_ARGS}
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