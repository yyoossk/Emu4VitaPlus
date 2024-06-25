cmake_minimum_required(VERSION 3.10)

set(CORE_NAME "fba_lite")
set(CORE_SHORT_NAME "FBALite")
set(CORE_FULL_NAME "FBA Lite")

if(TARGET ${CORE_NAME}_build)
  return()
endif()

project(${CORE_NAME})

set(APP_NAME "FbaLite_4Vita")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/libretro-fba-lite)
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/makefile.libretro)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_FULL_NAME})")
set(SFO_TITLE_ID "FBALITE4V")

set(CORE_ARGS platform=vita EXTERNAL_ZLIB=1)
set(CORE_SOFTWARE "Arcade (CPS1 2 3, NEO GEO, PGM)")

set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)
file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

add_compile_definitions(FBA_LITE_BUILD ARC_BUILD FBA_BUILD CONSOLE="ARC")
add_subdirectory(../../frontend ${CMAKE_CURRENT_BINARY_DIR}/frontend)