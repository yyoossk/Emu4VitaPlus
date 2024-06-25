cmake_minimum_required(VERSION 3.10)

set(CORE_NAME "mednafen_pce_fast")
set(CORE_SHORT_NAME "MednafenPCEFast")
set(CORE_FULL_NAME "Mednafen PCE Fast")

if(TARGET ${CORE_NAME}_build)
  return()
endif()

project(${CORE_NAME})

set(APP_NAME "${CORE_SHORT_NAME}_4Vita")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/beetle-pce-fast-libretro)
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Makefile)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_FULL_NAME})")
set(SFO_TITLE_ID "MPCEF4VIT")

set(CORE_ARGS platform=vita SYSTEM_ZLIB=1)
set(CORE_SOFTWARE "NEC - PC Engine / CD")

set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)
file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

add_compile_definitions(MEDNAFEN_PCE_FAST_BUILD PCE_BUILD CONSOLE="PCE")
add_subdirectory(../../frontend ${CMAKE_CURRENT_BINARY_DIR}/frontend)
