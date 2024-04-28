#pragma once

#define APP_ASSETS_DIR "app0:assets"
#define APP_DATA_DIR "ux0:data/EMU4VITAPLUS/" APP_DIR_NAME
#define APP_LOG_PATH APP_DATA_DIR "/app_log.txt"
#define CORE_DIR APP_DATA_DIR "/system"
#define TEXT_FONT_NAME "wqy-microhei.ttf"
#define GAMEPAD_FONT_NAME "promptfont.ttf"
#define APP_CONFIG_PATH APP_DATA_DIR "/config.toml"

#define MAIN_WINDOW_PADDING 10

#define TEXT(I) ((I) < TEXT_COUNT ? gTexts[gConfig->language][I] : "Unknown")

#define APP_STATUS_SHOW_UI 1
#define APP_STATUS_RUN_GAME (1 << 1)
#define APP_STATUS_EXIT (1 << 2)

#include "language_define.h"
#include "emulator.h"
#include "ui.h"
#include "config.h"

extern Ui *gUi;
extern Emulator *gEmulator;
extern uint32_t gStatus;
extern Config *gConfig;
