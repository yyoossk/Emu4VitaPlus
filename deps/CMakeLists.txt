cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

# add_custom_target(build_libretro ALL
# COMMAND make -f Makefile.libretro_comm
# )

# add_library(libretro_comm STATIC IMPORTED)
# set_target_properties(build_libretro
# IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/deps/libretro_comm/libretro_comm.a # Make sure to use absolute path here
# INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/deps/libretro_comm/
# #INTERFACE_COMPILE_DEFINITIONS "USING_THELIB;THELIB_STATIC"
# )
# add_dependencies(libretro_comm build_libretro)


set(SRC "libretro-common/string/stdstring.c"
    "libretro-common/encodings/encoding_utf.c"
    "libretro-common/encodings/encoding_crc32.c"
    "libretro-common/compat/fopen_utf8.c"
    "libretro-common/compat/compat_strl.c"
    "libretro-common/compat/compat_snprintf.c"
    "libretro-common/compat/compat_strcasestr.c"
    "libretro-common/compat/compat_posix_string.c"
    "libretro-common/file/file_path.c"
    "libretro-common/file/file_path_io.c"
    "libretro-common/streams/file_stream.c"
    "libretro-common/streams/file_stream_transforms.c"
    "libretro-common/streams/memory_stream.c"
    "libretro-common/lists/string_list.c"
    "libretro-common/lists/file_list.c"
    "libretro-common/lists/dir_list.c"
    "libretro-common/file/retro_dirent.c"
    "libretro-common/memmap/memalign.c"
    "libretro-common/vfs/vfs_implementation.c"
    "libretro-common/time/rtime.c"
    "libretro-common/streams/interface_stream.c"
    "libretro-common/streams/rzip_stream.c"
    "libretro-common/streams/stdin_stream.c"
    "libretro-common/streams/trans_stream.c"
    "libretro-common/streams/trans_stream_pipe.c"
    "libretro-common/streams/trans_stream_zlib.c"
    "libretro-common/features/features_cpu.c"
    "libretro-common/file/config_file.c"
    "libretro-common/file/config_file_userdata.c"
    "libretro-common/rthreads/rthreads.c"
    "libretro-common/utils/md5.c"
)

include_directories("libretro-common/include")

add_compile_definitions(HAVE_NEON HAVE_THREADS HAVE_COMPRESSION HAVE_ZLIB HAVE_7ZIP)
add_library(retro_comm STATIC ${SRC})