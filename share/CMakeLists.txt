cmake_minimum_required(VERSION 3.12)

set(CMAKE_DEPENDS_USE_COMPILER FALSE)
set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VITASDK}/share/vita.toolchain.cmake"
    CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

set(PUBLIC_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)

message(${PUBLIC_PKG_DIR})

file(GLOB_RECURSE PKG_FILES ${PUBLIC_PKG_DIR}/*)
set(PUBLIC_PKG_FILES "")

foreach(file ${PKG_FILES})
    file(RELATIVE_PATH rel ${PUBLIC_PKG_DIR} ${file})
    list(APPEND PUBLIC_PKG_FILES FILE ${file} ${rel})
endforeach()

SET(PUBLIC_PKG_FILES "${PUBLIC_PKG_FILES}" CACHE INTERNAL "PUBLIC_PKG_FILES")

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/source/language_define.cpp
    COMMAND python export.py
    COMMAND mv language_define.h ${CMAKE_CURRENT_SOURCE_DIR}/source
    COMMAND mv language_define.cpp ${CMAKE_CURRENT_SOURCE_DIR}/source
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../script
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../script/language.json
    ${CMAKE_CURRENT_SOURCE_DIR}/../script/translation.json
    ${CMAKE_CURRENT_SOURCE_DIR}/../script/export.py
)

file(GLOB SRC source/*.cpp)

if(NOT ${CMAKE_CURRENT_SOURCE_DIR}/source/language_define.cpp IN_LIST SRC)
    list(APPEND SRC ${CMAKE_CURRENT_SOURCE_DIR}/source/language_define.cpp)
endif()

include_directories(../deps/libretro-common/include)

add_library(share STATIC ${SRC})