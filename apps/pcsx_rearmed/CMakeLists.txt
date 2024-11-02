cmake_minimum_required(VERSION 3.10)

set(CORE_NAME "pcsx_rearmed")
set(CORE_SHORT_NAME "PCSXReARMed")
set(CORE_FULL_NAME "PCSX ReARMed")
set(CONSOLE "PS")

if(TARGET ${CORE_NAME}_build)
  return()
endif()

project(${CORE_NAME})

set(APP_NAME "${CORE_SHORT_NAME}_4Vita")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/${CORE_NAME})
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME Makefile.libretro)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_FULL_NAME})")
set(SFO_TITLE_ID "PCSX4VITA")

set(CORE_ARGS platform=vita DYNAREC=ari64)
set(CORE_SOFTWARE "Sony - PlayStation")

set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)
file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

add_compile_definitions(PCSX_REARMED_BUILD PS_BUILD)

add_subdirectory(.. ${CMAKE_CURRENT_BINARY_DIR}/app)
