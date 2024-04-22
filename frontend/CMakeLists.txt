cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/source/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/source/audio
)

file(GLOB src "*.c" "*.cpp" "*.h")
add_executable(${APP_NAME}.elf ${src})
target_link_libraries(${APP_NAME}.elf )