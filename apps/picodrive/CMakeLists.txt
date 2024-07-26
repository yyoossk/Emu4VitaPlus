cmake_minimum_required(VERSION 3.10)

set(CORE_NAME "picodrive")
set(CORE_SHORT_NAME "PicoDrive")
set(CORE_FULL_NAME "PicoDrive")

if(TARGET ${CORE_NAME}_build)
  return()
endif()

project(${CORE_NAME})

set(APP_NAME "${CORE_SHORT_NAME}_4Vita")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/picodrive)
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Makefile.libretro)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_FULL_NAME})")
set(SFO_TITLE_ID "PICODR4VT")

set(CORE_ARGS platform=vita EXTERNAL_ZLIB=0)
set(CORE_SOFTWARE "Sega - MS/GG/MD/CD")

set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)
file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

add_compile_definitions(PICODRIVE_BUILD SMS_BUILD MD_BUILD MDCD_BUILD MD32X_BUILD CONSOLE="MD")

add_subdirectory(.. ${CMAKE_CURRENT_BINARY_DIR}/app)
