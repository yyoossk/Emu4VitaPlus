cmake_minimum_required(VERSION 3.10)

set(CORE_NAME "fbalpha2012")
set(CORE_SHORT_NAME "FBA2012")
set(CORE_FULL_NAME "FBA 2012")
set(CONSOLE "ARC")

if(TARGET ${CORE_NAME}_build)
  return()
endif()

project(${CORE_NAME})

set(APP_NAME "Fba2012_4Vita")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/fbalpha2012/svn-current/trunk)
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/makefile.libretro)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_FULL_NAME})")
set(SFO_TITLE_ID "FBA20124V")

set(CORE_ARGS platform=vita EXTERNAL_ZLIB=1 HAVE_NEON=1)
set(CORE_SOFTWARE "Arcade (CPS1 2 3, NEO GEO, PGM)")

set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)
file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

add_compile_definitions(FBA_2012_BUILD ARC_BUILD FBA_BUILD)

add_subdirectory(.. ${CMAKE_CURRENT_BINARY_DIR}/app)