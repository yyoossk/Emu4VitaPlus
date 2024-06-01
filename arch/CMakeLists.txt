cmake_minimum_required(VERSION 3.12)

set(CMAKE_DEPENDS_USE_COMPILER FALSE)
set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VITASDK}/share/vita.toolchain.cmake"
    CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

set(APP_NAME "Emu4VitaPlus")
set(APP_DIR_NAME "Arch")
set(SFO_TITLE_NAME "Emu4Vita++")
set(SFO_TITLE_ID "EMU4VPLUS")
set(PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pkg)

file(GLOB_RECURSE PKG_FILES ${PKG_DIR}/*)
set(PRIVATE_PKG_FILES "")

foreach(file ${PKG_FILES})
    file(RELATIVE_PATH rel ${PKG_DIR} ${file})
    list(APPEND PRIVATE_PKG_FILES FILE ${file} ${rel})
endforeach()

project("Arch")

set(BUILD_SELF ON)
add_subdirectory(../apps/gpsp gpsp)
add_subdirectory(../apps/fba_lite fba_lite)

if(${CMAKE_VERSION} VERSION_LESS "3.20.0")
    set(CORE_EBOOTS "eboot_gpSP4vita.bin" "eboot_FbaL4vita.bin")
else()
    set(CORE_EBOOTS "eboot_gpSP4vita.bin-self" "eboot_FbaL4vita.bin-self")
endif()


include_directories(
    source
    ../share/source
    ../deps/libvita2d/libvita2d/include
    ../deps/simpleini)

file(GLOB SRC
    source/*.cpp
    source/ui/*.cpp
    source/audio/*.cpp
    ../share/source/*.cpp)
add_executable(${APP_NAME}.elf ${SRC})

target_link_libraries(
    ${APP_NAME}.elf
    share
    vita2d
    imgui_vita2d
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
vita_create_self(eboot_${APP_NAME}.bin ${APP_NAME}.elf UNSAFE STRIPPED REL_OPTIMIZE)

set(VPK_NAME ${APP_NAME}_${APP_VER}.vpk)
set(VITA_MKSFOEX_FLAGS -d ATTRIBUTE2=12)
vita_create_vpk(
    ${VPK_NAME}
    ${SFO_TITLE_ID}
    eboot_${APP_NAME}.bin
    VERSION
    ${SFO_APP_VER}
    NAME
    "${SFO_TITLE_NAME}"
    ${PRIVATE_PKG_FILES}
    ${PUBLIC_PKG_FILES})

add_custom_target(VPK_${APP_NAME} ALL
    COMMAND ${CMAKE_COMMAND} -E rename ${VPK_NAME} ${OUT_PATH}/${VPK_NAME})

if(${CMAKE_VERSION} VERSION_LESS "3.20.0")
    add_dependencies(${VPK_NAME} ${CORE_EBOOTS})
    add_dependencies(VPK_${APP_NAME} ${VPK_NAME})
else()
    add_dependencies(${VPK_NAME}-vpk ${CORE_EBOOTS})
    add_dependencies(VPK_${APP_NAME} ${VPK_NAME}-vpk)
endif()