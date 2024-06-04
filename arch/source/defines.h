#pragma once

#define APP_ASSETS_DIR "app0:assets"
#define CORE_DATA_DIR "app0:data"
#define APP_DATA_DIR "ux0:data/EMU4VITAPLUS/" APP_DIR_NAME
#define APP_LOG_PATH APP_DATA_DIR "/Emu4Vita++.log"
#define APP_CONFIG_PATH APP_DATA_DIR "/config.ini"

#define LOOP_PLUS_ONE(VALUE, TOTAL) (VALUE = ((VALUE + 1 == (TOTAL)) ? 0 : VALUE + 1))
#define LOOP_MINUS_ONE(VALUE, TOTAL) (VALUE = ((VALUE == 0) ? (TOTAL) - 1 : VALUE - 1))

#define BUTTON_SIZE 160