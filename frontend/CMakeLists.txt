cmake_minimum_required(VERSION 3.12)

add_custom_target(
    ${CORE_NAME}_build ALL
    COMMAND make -f ${CORE_MAKEFILE_NAME} ${CORE_ARGS} -j4
    WORKING_DIRECTORY ${CORE_DIR})

add_library(${CORE_NAME} STATIC IMPORTED)
set_property(
    TARGET ${CORE_NAME}
    APPEND
    PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)

set(LIBRETRO_LIBRARY ${CORE_DIR}/${CORE_NAME}_libretro_vita.a)
set_target_properties(${CORE_NAME} PROPERTIES IMPORTED_LOCATION_NOCONFIG
    "${LIBRETRO_LIBRARY}")
add_dependencies(${CORE_NAME} ${CORE_NAME}_build)

if(CORE_NAME STREQUAL "fbneo")
    set(CMAKE_C_FLAGS " -marm -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard -DVITA -D__vita__ -O3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOG_LEVEL=${LOG_LEVEL}")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-q")
endif()

include_directories(
    source
    source/ui
    source/audio
    source/emu
    ../share/source
    ../deps/libretro-common/include
    ../deps/libvita2d/libvita2d/include
    ../deps/libvita2d_ext/libvita2d_ext/include
    ../deps/simpleini
    ../deps/minizip-ng)

file(GLOB SRC
    source/*.c
    source/*.cpp
    source/ui/*.cpp
    source/emu/*.cpp
    source/audio/*.cpp)

add_compile_definitions(
    APP_NAME_STR="${APP_NAME}"
    APP_NAME_EXT_STR=${APP_NAME_EXT}
    APP_VER_STR="${APP_VER}"
    APP_TITLEID=${SFO_TITLE_ID}
    APP_DIR_NAME="${CORE_NAME}"
    APP_TITLE_NAME="${SFO_TITLE_NAME}"
    CORE_FULL_NAME="${CORE_FULL_NAME}")

set(ELF_NAME ${CORE_SHORT_NAME}.elf)
set(EBOOT_NAME eboot_${CORE_SHORT_NAME}.bin)

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
    vita2d_ext
    imgui_vita2d
    swresample
    swscale
    avutil
    SimpleIni
    png
    jpeg
    z
    lzma
    minizip
    7zip
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