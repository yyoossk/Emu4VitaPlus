cmake_minimum_required(VERSION 3.10)

if(TARGET Snes9x2005_build)
  return()
endif()

set(APP_NAME "Snes9x2005_4Vita")
set(CORE_NAME "Snes9x2005")
set(CORE_FULL_NAME "Snes9x 2005")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/${CORE_NAME})
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME Makefile)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_NAME})")
set(SFO_TITLE_ID "SNES054VT")
set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)

set(CORE_ARGS platform=vita EXTERNAL_ZLIB=1)

set(CORE_SOFTWARE "Nintendo - SNES / SFC")

file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

project("Snes9x2005")

add_custom_target(
  Snes9x2005_build ALL
  COMMAND make -f ${CORE_MAKEFILE_NAME} -j ${CORE_ARGS}
  WORKING_DIRECTORY ${CORE_DIR})

add_library(Snes9x2005 STATIC IMPORTED)
set_property(
  TARGET Snes9x2005
  APPEND
  PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)

set(LIBRETRO_LIBRARY ${CORE_DIR}/snes9x2005_libretro_vita.a)
set_target_properties(Snes9x2005 PROPERTIES IMPORTED_LOCATION_NOCONFIG
  "${LIBRETRO_LIBRARY}")
add_dependencies(Snes9x2005 Snes9x2005_build)

add_compile_definitions(SNES9X2005_BUILD SFC_BUILD)
add_subdirectory(../../frontend ${CMAKE_CURRENT_BINARY_DIR}/frontend)
