cmake_minimum_required(VERSION 3.10)

set(CORE_NAME "genesis_plus_gx_wide")
set(CORE_SHORT_NAME "GenesisPlusGXWide")
set(CORE_FULL_NAME "Genesis Plus GX Wide")
set(CONSOLE "MD")

if(TARGET ${CORE_NAME}_build)
  return()
endif()

project(${CORE_NAME})

set(APP_NAME "${CORE_SHORT_NAME}4VitaPlus")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/Genesis-Plus-GX-Wide)
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME Makefile.libretro)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_FULL_NAME})")
set(SFO_TITLE_ID "GENWIDE4V")

set(CORE_ARGS platform=vita EXTERNAL_ZLIB=1)
set(CORE_SOFTWARE "Sega - MS/GG/MD/CD")

set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)
file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

add_compile_definitions(GENESIS_PLUS_GX_WIDE_BUILD MD_BUILD)

add_subdirectory(.. ${CMAKE_CURRENT_BINARY_DIR}/app)
