#pragma once

#define APP_DATA_DIR "ux0:data/EMU4VITAPLUS/" APP_DIR_NAME
#define APP_LOG_PATH APP_DATA_DIR "/Emu4Vita++.log"
#define APP_CONFIG_PATH APP_DATA_DIR "/config.ini"
#define APP_CORE_CONFIG_PATH APP_DATA_DIR "/core.ini"

#define CORE_SYSTEM_DIR APP_DATA_DIR "/system"
#define CORE_SAVEFILES_DIR APP_DATA_DIR "/savefiles"
#define CORE_SAVESTATES_DIR APP_DATA_DIR "/savestates"

#define OVERLAYS_DIR_NAME "overlays"
#define PREVIEW_DIR_NAME "images"

#define LOOP_PLUS_ONE(VALUE, TOTAL) (VALUE = ((VALUE + 1 == (TOTAL)) ? 0 : VALUE + 1))
#define LOOP_MINUS_ONE(VALUE, TOTAL) (VALUE = ((VALUE == 0) ? (TOTAL) - 1 : VALUE - 1))
