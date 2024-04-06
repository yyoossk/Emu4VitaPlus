#pragma once
#include "emulator.h"
#include "ui.h"
#include "config.h"

#define APP_ASSETS_DIR "app0:assets"
#define APP_DATA_DIR "ux0:data/EMU4VITAPLUS/" APP_DIR_NAME
#define APP_LOG_PATH APP_DATA_DIR "/app_log.txt"
#define CORE_DIR APP_DATA_DIR "/system"
#define FONT_PVF_NAME "wqy-microhei.ttf"
#define APP_CONFIG_PATH APP_DATA_DIR "/config.toml"

#define MAIN_WINDOW_PADDING 10

enum APP_STATUS
{
    APP_STATUS_SHOW_BROWSER,
    APP_STATUS_RUN_GAME,
    APP_STATUS_EXIT
};

extern Ui *gUi;
extern Emulator *gEmulator;
extern APP_STATUS gStatus;
extern Config *gConfig;