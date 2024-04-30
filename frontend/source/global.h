#pragma once

#define APP_ASSETS_DIR "app0:assets"
#define APP_DATA_DIR "ux0:data/EMU4VITAPLUS/" APP_DIR_NAME
#define APP_LOG_PATH APP_DATA_DIR "/app_log.txt"
#define CORE_DIR APP_DATA_DIR "/system"
#define TEXT_FONT_NAME "wqy-microhei.ttf"
#define GAMEPAD_FONT_NAME "promptfont.ttf"
#define APP_CONFIG_PATH APP_DATA_DIR "/config.toml"
#define OVERLAYS_DIR_NAME "overlays"
#define PREVIEW_DIR_NAME "images"

#define MAIN_WINDOW_PADDING 10

enum APP_STATUS
{
    APP_STATUS_SHOW_UI,
    APP_STATUS_RUN_GAME,
    APP_STATUS_SHOW_UI_IN_GAME,
    APP_STATUS_EXIT
};

#include "language_define.h"
#include "config.h"

extern Config *gConfig;
#define TEXT(I) ((I) < TEXT_COUNT ? gTexts[gConfig->language][I] : "Unknown")

#include "emulator.h"
#include "ui.h"

extern Ui *gUi;
extern Emulator *gEmulator;
extern APP_STATUS gStatus;
