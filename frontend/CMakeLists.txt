cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VITASDK}/share/vita.toolchain.cmake"
    CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

set(APP_VER "0.1")
set(SFO_APP_VER "00.01")
set(PUBLIC_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)

file(GLOB_RECURSE PKG_FILES ${PUBLIC_PKG_DIR}/*)
set(PUBLIC_PKG_FILES "")

foreach(file ${PKG_FILES})
    file(RELATIVE_PATH rel ${PUBLIC_PKG_DIR} ${file})
    list(APPEND PUBLIC_PKG_FILES FILE ${file} ${rel})
endforeach()

include_directories(
    source source/ui
    source/audio
    ../deps/libretro-common/include
    ../deps/libvita2d/libvita2d/include
    ../deps/tomlplusplus/include)

file(GLOB SRC
    source/*.c
    source/*.cpp
    source/ui/*.cpp
    source/audio/*.cpp)

set(INC frontend/source
    frontend/source/ui
    frontend/source/audio
    deps/libretro-common/include
    deps/tomlplusplus/include)

add_compile_definitions(
    APP_NAME_STR="${APP_NAME}"
    APP_NAME_EXT_STR=${APP_NAME_EXT}
    APP_VER_STR=${APP_VER}
    APP_TITLEID=${SFO_TITLE_ID}
    APP_DIR_NAME="${APP_DIR_NAME}"
    CORE_SOFTWARE=${CORE_SOFTWARE}
    TOML_EXCEPTIONS=0)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,-q")
add_executable(${APP_NAME}.elf ${SRC})

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/source/language_define.cpp
    COMMAND python export.py
    COMMAND mv language_define.h ${CMAKE_CURRENT_SOURCE_DIR}/source
    COMMAND mv language_define.cpp ${CMAKE_CURRENT_SOURCE_DIR}/source
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../script
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../script/language.json)

target_link_libraries(
    ${APP_NAME}.elf
    ${LIBRETRO_LIBRARY}
    retro_comm
    vita2d
    imgui_vita2d
    swresample
    avutil
    png
    jpeg
    z
    pthread
    ScePvf_stub
    ScePgf_stub
    SceDisplay_stub
    SceGxm_stub
    SceCtrl_stub
    SceTouch_stub
    ScePower_stub
    SceAudio_stub
    SceRtc_stub
    SceCommonDialog_stub
    SceSysmodule_stub
    SceAppUtil_stub
    SceAppMgr_stub
    SceShellSvc_stub
    SceMotion_stub
    SceHid_stub
    SceFiber_stub)

set(VITA_MAKE_FSELF_FLAGS "-a 0x2800000000000001")
vita_create_self(eboot.bin ${APP_NAME}.elf UNSAFE STRIPPED REL_OPTIMIZE)

if(WITH_LOG)
    set(VPK_NAME ${APP_NAME}_${APP_VER}_log.vpk)
else()
    set(VPK_NAME ${APP_NAME}_${APP_VER}.vpk)
endif()

set(VITA_MKSFOEX_FLAGS -d ATTRIBUTE2=12)
vita_create_vpk(
    ${VPK_NAME}
    ${SFO_TITLE_ID}
    eboot.bin
    VERSION
    ${SFO_APP_VER}
    NAME
    "${SFO_TITLE_NAME}"
    ${PRIVATE_PKG_FILES}
    ${PUBLIC_PKG_FILES})

add_custom_target(VPK ALL
    COMMAND ${CMAKE_COMMAND} -E rename ${VPK_NAME} ${OUT_PATH}/${VPK_NAME})

if(${CMAKE_VERSION} VERSION_LESS "3.20.0")
    add_dependencies(VPK ${VPK_NAME})
else()
    add_dependencies(VPK ${VPK_NAME}-vpk)
endif()