TARGET = gpSP4vita

APP_NAME         := gpSP4vita
APP_NAME_EXT     := 
APP_DIR_NAME     := gpSP
CORE_FILE_NAME   := gpSP

SFO_TITLE_NAME   := Emu4Vita++ (gpSP)
SFO_TITLE_ID     := GPSP4VITA

CORE_SOFTWARE    := Nintendo\ -\ Game\ Boy\ Advance

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
APP_DIR            := $(dir $(MAKEFILE_PATH))
ROOT_DIR           := $(APP_DIR)/../../
FRONTEND_DIR       := $(ROOT_DIR)/frontend
CORE_DIR           := $(ROOT_DIR)/cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)
CORE_MAKEFILE_NAME := Makefile

WANT_DISPLAY_ROTATE  :=
WANT_EXT_ARCHIVE_ROM := 1

CORE_ARGS := 

CORE_DEFINES := \
	-DGPSP_BUILD \
	-DGBA_BUILD

include $(FRONTEND_DIR)/Makefile.common
