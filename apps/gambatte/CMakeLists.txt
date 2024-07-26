cmake_minimum_required(VERSION 3.10)

set(CORE_NAME "gambatte")
set(CORE_SHORT_NAME "Gambatte")
set(CORE_FULL_NAME "Gambatte")

if(TARGET ${CORE_NAME}_build)
  return()
endif()

project(${CORE_NAME})

set(APP_NAME "${CORE_SHORT_NAME}_4Vita")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/gambatte-libretro)
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME Makefile.libretro)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_FULL_NAME})")
set(SFO_TITLE_ID "GAMBATTEV")

set(CORE_ARGS platform=vita EXTERNAL_ZLIB=1)
set(CORE_SOFTWARE "Nintendo - Game Boy Color")

set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)
file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

add_compile_definitions(GAMEBATTE_BUILD GBC_BUILD CONSOLE="GBC")

add_subdirectory(.. ${CMAKE_CURRENT_BINARY_DIR}/app)
