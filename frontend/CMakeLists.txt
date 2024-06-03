cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VITASDK}/share/vita.toolchain.cmake"
    CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

include_directories(
    source
    source/ui
    source/audio
    ../share/source
    ../deps/libretro-common/include
    ../deps/libvita2d/libvita2d/include
    ../deps/simpleini)

file(GLOB SRC
    source/*.c
    source/*.cpp
    source/ui/*.cpp
    source/audio/*.cpp)

add_compile_definitions(
    APP_NAME_STR="${APP_NAME}"
    APP_NAME_EXT_STR=${APP_NAME_EXT}
    APP_VER_STR=${APP_VER}
    APP_TITLEID=${SFO_TITLE_ID}
    APP_DIR_NAME="${APP_DIR_NAME}"
    APP_TITLE_NAME="${SFO_TITLE_NAME}"
    CORE_SOFTWARE=${CORE_SOFTWARE})

set(ELF_NAME ${APP_NAME}.elf)
set(EBOOT_NAME eboot_${CORE_NAME}.bin)

if(WITH_LOG)
    set(VPK_NAME ${APP_NAME}_${APP_VER}_log.vpk)
else()
    set(VPK_NAME ${APP_NAME}_${APP_VER}.vpk)
endif()

add_executable(${ELF_NAME} ${SRC})

target_link_libraries(
    ${ELF_NAME}
    ${LIBRETRO_LIBRARY}
    share
    retro_comm
    vita2d
    imgui_vita2d
    swresample
    swscale
    avutil
    SimpleIni
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
vita_create_self(${EBOOT_NAME} ${ELF_NAME} STRIPPED) # UNSAFE STRIPPED REL_OPTIMIZE)

if(NOT BUILD_SELF)
    set(VITA_MKSFOEX_FLAGS -d ATTRIBUTE2=12)
    vita_create_vpk(
        ${VPK_NAME}
        ${SFO_TITLE_ID}
        ${EBOOT_NAME}
        VERSION
        ${SFO_APP_VER}
        NAME
        "${SFO_TITLE_NAME}"
        ${PRIVATE_PKG_FILES}
        ${PUBLIC_PKG_FILES})

    add_custom_target(VPK_${APP_NAME} ALL
        COMMAND ${CMAKE_COMMAND} -E rename ${VPK_NAME} ${OUT_PATH}/${VPK_NAME})

    if(${CMAKE_VERSION} VERSION_LESS "3.20.0")
        add_dependencies(VPK_${APP_NAME} ${VPK_NAME})
    else()
        add_dependencies(VPK_${APP_NAME} ${VPK_NAME}-vpk)
    endif()
endif()