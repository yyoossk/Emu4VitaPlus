cmake_minimum_required(VERSION 3.10)

if(TARGET fba_lite_build)
  return()
endif()

set(CMAKE_TOOLCHAIN_FILE
  "$ENV{VITASDK}/share/vita.toolchain.cmake"
  CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

set(APP_NAME "FbNeovita")
set(APP_DIR_NAME "fbneo")
set(CORE_NAME "fbneo")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/FBNeo)
set(CORE_MAKEFILE_DIR ${CORE_DIR}/src/burner/libretro)
set(CORE_MAKEFILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Makefile)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_NAME})")
set(SFO_TITLE_ID "FBNEO4VIT")
set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)

set(CORE_ARGS platform=vita EXTERNAL_ZLIB=1 INCLUDE_7Z_SUPPORT=0)

set(CORE_SOFTWARE, "Arcade (CPS1 2 3, NEO GEO, PGM)")

file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

project("fbneo")

set(SRC_DIR)
add_custom_target(
  fbneo_build ALL
  COMMAND alias gcc="${CMAKE_C_COMPILER}"
  COMMAND make -f ${CORE_MAKEFILE_NAME} -C ${CORE_MAKEFILE_DIR} -j ${CORE_ARGS} generate-files
  COMMAND make -f ${CORE_MAKEFILE_NAME} -C ${CORE_MAKEFILE_DIR} -j ${CORE_ARGS}
  WORKING_DIRECTORY ${CORE_DIR})

add_library(fbneo STATIC IMPORTED)
set_property(
  TARGET fbneo
  APPEND
  PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)

set(LIBRETRO_LIBRARY ${CORE_MAKEFILE_DIR}/fbneo_libretro_vita.a)
set_target_properties(fbneo PROPERTIES IMPORTED_LOCATION_NOCONFIG
  "${LIBRETRO_LIBRARY}")
add_dependencies(fbneo fbneo_build)

add_compile_definitions(FBNEO_BUILD ARC_BUILD FBA_BUILD)
add_subdirectory(../../frontend ${CMAKE_CURRENT_BINARY_DIR}/frontend)