cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(APP_NAME "gpSP4vita")
set(APP_DIR_NAME, "gpSP")
set(CORE_FILE_NAME, "gpSP")
set(SFO_TITLE_NAME, "Emu4Vita++ (${CORE_FILE_NAME})")
set(SFO_TITLE_ID,   "GPSP4VITA")

set(CORE_SOFTWARE, "Nintendo - Game Boy Advance")

project("gpSP")

add_subdirectory(../../frontend)

