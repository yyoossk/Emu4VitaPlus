cmake_minimum_required(VERSION 3.10)

if(TARGET Nestopia_build)
  return()
endif()

set(CMAKE_TOOLCHAIN_FILE
  "$ENV{VITASDK}/share/vita.toolchain.cmake"
  CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

set(APP_NAME "Nestopia_4Vita")
set(CORE_NAME "Nestopia")
set(CORE_FULL_NAME "Nestopia")
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cores/nestopia/libretro)
set(CORE_MAKEFILE_DIR ${CORE_DIR})
set(CORE_MAKEFILE_NAME Makefile)
set(SFO_TITLE_NAME "Emu4Vita++ (${CORE_NAME})")
set(SFO_TITLE_ID "NESTP4VIT")
set(PRIVATE_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)

set(CORE_ARGS platform=vita EXTERNAL_ZLIB=1)

set(CORE_SOFTWARE "Nintendo NES Famicom")

file(GLOB_RECURSE PKG_FILES ${PRIVATE_PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
  file(RELATIVE_PATH rel ${PRIVATE_PKG_DIR} ${file})
  list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

project(${CORE_NAME})

add_custom_target(
  ${CORE_NAME}_build ALL
  COMMAND make -f ${CORE_MAKEFILE_NAME} -j ${CORE_ARGS}
  WORKING_DIRECTORY ${CORE_DIR})

add_library(${CORE_NAME} STATIC IMPORTED)
set_property(
  TARGET ${CORE_NAME}
  APPEND
  PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)

set(LIBRETRO_LIBRARY ${CORE_DIR}/nestopia_libretro_vita.a)
set_target_properties(${CORE_NAME} PROPERTIES IMPORTED_LOCATION_NOCONFIG
  "${LIBRETRO_LIBRARY}")
add_dependencies(${CORE_NAME} ${CORE_NAME}_build)

add_compile_definitions(NESTOPIA_BUILD FC_BUILD)
add_subdirectory(../../frontend ${CMAKE_CURRENT_BINARY_DIR}/frontend)
