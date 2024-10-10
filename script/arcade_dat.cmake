cmake_minimum_required(VERSION 3.10)

macro(gen_arc_dat OUT_PATH)
    set(ARC_DAT_PATH ${OUT_PATH}/assets/arcade_dat.bin)
    file(RELATIVE_PATH rel ${OUT_PATH} ${ARC_DAT_PATH})
    list(APPEND PRIVATE_PKG_FILES FILE ${ARC_DAT_PATH} ${rel})

    set(ARC_SCRIPT grab_arc_dat.py)
    set(ARC_TAG ARC_DAT_BUILD)
    add_custom_command(
        OUTPUT
        ${ARC_DAT_PATH}

        COMMAND python ${ARC_SCRIPT}
        COMMAND mv arcade_dat.bin ${ARC_DAT_PATH}

        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/script

        DEPENDS
        ${CMAKE_SOURCE_DIR}/script/${ARC_SCRIPT}
        ${CMAKE_SOURCE_DIR}/cores/libretro-fbneo/dats/*
    )

    add_custom_target(${ARC_TAG}
        DEPENDS
        ${ARC_DAT_PATH})
endmacro()