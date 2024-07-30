#include <psp2/ctrl.h>
#include <psp2/apputil.h>
#include <psp2/system_param.h>
#include <libretro.h>
#include <stdio.h>
#include <unordered_map>
#include <string>
#include "SimpleIni.h"
#include "config.h"
#include "input.h"
#include "file.h"
#include "log.h"
#include "defines.h"

#define MAIN_SECTION "MAIN"
#define HOTKEY_SECTION "HOTKEY"
#define GRAPHICS_SECTION "GRAPHICS"

Config *gConfig = nullptr;

#define KEY_PAIR(K) \
    {               \
        K, #K}

const std::unordered_map<uint32_t, const char *> Config::PsvKeyStr = {
    KEY_PAIR(SCE_CTRL_CROSS),
    KEY_PAIR(SCE_CTRL_TRIANGLE),
    KEY_PAIR(SCE_CTRL_CIRCLE),
    KEY_PAIR(SCE_CTRL_SQUARE),
    KEY_PAIR(SCE_CTRL_SELECT),
    KEY_PAIR(SCE_CTRL_START),
    KEY_PAIR(SCE_CTRL_UP),
    KEY_PAIR(SCE_CTRL_DOWN),
    KEY_PAIR(SCE_CTRL_LEFT),
    KEY_PAIR(SCE_CTRL_RIGHT),
    KEY_PAIR(SCE_CTRL_L1),
    KEY_PAIR(SCE_CTRL_R1),
    KEY_PAIR(SCE_CTRL_L2),
    KEY_PAIR(SCE_CTRL_R2),
    KEY_PAIR(SCE_CTRL_L3),
    KEY_PAIR(SCE_CTRL_R3),
    KEY_PAIR(SCE_CTRL_LSTICK_UP),
    KEY_PAIR(SCE_CTRL_LSTICK_DOWN),
    KEY_PAIR(SCE_CTRL_LSTICK_LEFT),
    KEY_PAIR(SCE_CTRL_LSTICK_RIGHT),
    KEY_PAIR(SCE_CTRL_RSTICK_UP),
    KEY_PAIR(SCE_CTRL_RSTICK_DOWN),
    KEY_PAIR(SCE_CTRL_RSTICK_LEFT),
    KEY_PAIR(SCE_CTRL_RSTICK_RIGHT),
};

const std::unordered_map<uint32_t, const char *> Config::HotkeyStr = {
    KEY_PAIR(SAVE_STATE),
    KEY_PAIR(LOAD_STATE),
    KEY_PAIR(GAME_SPEED_UP),
    KEY_PAIR(GAME_SPEED_DOWN),
    KEY_PAIR(GAME_REWIND),
    KEY_PAIR(CONTROLLER_PORT_UP),
    KEY_PAIR(CONTROLLER_PORT_DOWN),
    KEY_PAIR(EXIT_GAME),
};

const std::unordered_map<uint32_t, const char *> Config::GraphicsStr = {
    KEY_PAIR(DISPLAY_SIZE),
    KEY_PAIR(DISPLAY_RATIO),
    KEY_PAIR(DISPLAY_ROTATE),
    KEY_PAIR(GRAPHICS_SHADER),
    KEY_PAIR(GRAPHICS_SMOOTH),
    KEY_PAIR(GRAPHICS_OVERLAY),
    KEY_PAIR(GRAPHICS_OVERLAY_MODE),
};

const std::unordered_map<uint32_t, TEXT_ENUM> Config::ControlTextMap = {
    {SCE_CTRL_CROSS, BUTTON_CROSS},
    {SCE_CTRL_TRIANGLE, BUTTON_TRIANGLE},
    {SCE_CTRL_CIRCLE, BUTTON_CIRCLE},
    {SCE_CTRL_SQUARE, BUTTON_SQUARE},
    {SCE_CTRL_SELECT, BUTTON_SELECT},
    {SCE_CTRL_START, BUTTON_START},
    {SCE_CTRL_UP, BUTTON_UP},
    {SCE_CTRL_DOWN, BUTTON_DOWN},
    {SCE_CTRL_LEFT, BUTTON_LEFT},
    {SCE_CTRL_RIGHT, BUTTON_RIGHT},
    {SCE_CTRL_L1, BUTTON_L1},
    {SCE_CTRL_R1, BUTTON_R1},
    {SCE_CTRL_L2, BUTTON_L2},
    {SCE_CTRL_R2, BUTTON_R2},
    {SCE_CTRL_L3, BUTTON_L3},
    {SCE_CTRL_R3, BUTTON_R3},
    {SCE_CTRL_LSTICK_UP, BUTTON_LEFT_ANALOG_UP},
    {SCE_CTRL_LSTICK_DOWN, BUTTON_LEFT_ANALOG_DOWN},
    {SCE_CTRL_LSTICK_LEFT, BUTTON_LEFT_ANALOG_LEFT},
    {SCE_CTRL_LSTICK_RIGHT, BUTTON_LEFT_ANALOG_RIGHT},
    {SCE_CTRL_RSTICK_UP, BUTTON_RIGHT_ANALOG_UP},
    {SCE_CTRL_RSTICK_DOWN, BUTTON_RIGHT_ANALOG_DOWN},
    {SCE_CTRL_RSTICK_LEFT, BUTTON_RIGHT_ANALOG_LEFT},
    {SCE_CTRL_RSTICK_RIGHT, BUTTON_RIGHT_ANALOG_RIGHT},
    {SCE_CTRL_PSBUTTON, BUTTON_HOME},
};

const std::unordered_map<uint8_t, TEXT_ENUM> Config::RetroTextMap = {
    {RETRO_DEVICE_ID_JOYPAD_B, BUTTON_B},
    {RETRO_DEVICE_ID_JOYPAD_Y, BUTTON_Y},
    {RETRO_DEVICE_ID_JOYPAD_SELECT, BUTTON_SELECT},
    {RETRO_DEVICE_ID_JOYPAD_START, BUTTON_START},
    {RETRO_DEVICE_ID_JOYPAD_UP, BUTTON_UP},
    {RETRO_DEVICE_ID_JOYPAD_DOWN, BUTTON_DOWN},
    {RETRO_DEVICE_ID_JOYPAD_LEFT, BUTTON_LEFT},
    {RETRO_DEVICE_ID_JOYPAD_RIGHT, BUTTON_RIGHT},
    {RETRO_DEVICE_ID_JOYPAD_A, BUTTON_A},
    {RETRO_DEVICE_ID_JOYPAD_X, BUTTON_X},
    {RETRO_DEVICE_ID_JOYPAD_L, BUTTON_L},
    {RETRO_DEVICE_ID_JOYPAD_R, BUTTON_R},
    {RETRO_DEVICE_ID_JOYPAD_L2, BUTTON_L2},
    {RETRO_DEVICE_ID_JOYPAD_R2, BUTTON_R2},
    {RETRO_DEVICE_ID_JOYPAD_L3, BUTTON_L3},
    {RETRO_DEVICE_ID_JOYPAD_R3, BUTTON_R3},
    {RETRO_DEVICE_ID_NONE, NONE},
};

Config::Config()
    : language(LANGUAGE_ENGLISH),
      hotkeys{0},
      boot_from_arch(false)
{
    LogFunctionName;
    Default();
}

Config::~Config()
{
    LogFunctionName;
    Save();
}

void Config::Default()
{
    LogFunctionName;

    rewind = 1;
    rewind_buf_size = DEFAULT_REWIND_BUF_SIZE;
    last_rom = "ux0:";
    mute = false;

    int sys_lang;
    sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, &sys_lang);
    switch (sys_lang)
    {
    case SCE_SYSTEM_PARAM_LANG_CHINESE_S:
    case SCE_SYSTEM_PARAM_LANG_CHINESE_T:
        language = LANGUAGE_CHINESE;
        break;
    case SCE_SYSTEM_PARAM_LANG_ENGLISH_US:
    case SCE_SYSTEM_PARAM_LANG_ENGLISH_GB:
    default:
        language = LANGUAGE_ENGLISH;
        break;
    }

    DefaultControlMap();
    DefaultHotKey();
    DefaultGraphics();
}

void Config::DefaultControlMap()
{
    LogFunctionName;
    control_maps = CONTROL_MAPS;
}

void Config::DefaultHotKey()
{
    LogFunctionName;
    hotkeys[SAVE_STATE] = SCE_CTRL_PSBUTTON | SCE_CTRL_SQUARE;
    hotkeys[LOAD_STATE] = SCE_CTRL_PSBUTTON | SCE_CTRL_TRIANGLE;
    hotkeys[GAME_SPEED_UP] = SCE_CTRL_PSBUTTON | SCE_CTRL_R1;
    hotkeys[GAME_SPEED_DOWN] = SCE_CTRL_PSBUTTON | SCE_CTRL_L1;
    hotkeys[GAME_REWIND] = SCE_CTRL_PSBUTTON | SCE_CTRL_RSTICK_LEFT;
    hotkeys[CONTROLLER_PORT_UP] = SCE_CTRL_PSBUTTON | SCE_CTRL_RSTICK_UP;
    hotkeys[CONTROLLER_PORT_DOWN] = SCE_CTRL_PSBUTTON | SCE_CTRL_RSTICK_DOWN;
    hotkeys[EXIT_GAME] = SCE_CTRL_PSBUTTON | SCE_CTRL_CROSS;
}

void Config::DefaultGraphics()
{
    LogFunctionName;
    graphics[DISPLAY_SIZE] = CONFIG_DISPLAY_SIZE_FULL;
    graphics[DISPLAY_RATIO] = CONFIG_DISPLAY_RATIO_BY_GAME_RESOLUTION;
    graphics[DISPLAY_ROTATE] = CONFIG_DISPLAY_ROTATE_DEFAULT;
    graphics[GRAPHICS_SHADER] = 0;
    graphics[GRAPHICS_SMOOTH] = CONFIG_GRAPHICS_SMOOTHER_NO;
    graphics[GRAPHICS_OVERLAY_MODE] = CONFIG_GRAPHICS_OVERLAY_MODE_OVERLAY;
    graphics[GRAPHICS_OVERLAY] = 0;
}

bool Config::Save(const char *path)
{
    LogFunctionName;
    LogDebug("path: %s", path);

    CSimpleIniA ini;

    ini.SetValue(MAIN_SECTION, "language", gLanguageNames[language]);
    ini.SetLongValue(MAIN_SECTION, "rewind", rewind);
    ini.SetLongValue(MAIN_SECTION, "rewind_buf_size", rewind_buf_size);
    ini.SetValue(MAIN_SECTION, "last_rom", last_rom.c_str());

    for (const auto &control : control_maps)
    {
        ini.SetLongValue(PsvKeyStr.at(control.psv), "retro", control.retro);
        ini.SetBoolValue(PsvKeyStr.at(control.psv), "turbo", control.turbo);
    }

    for (size_t i = 0; i < HOT_KEY_COUNT; i++)
    {
        ini.SetLongValue(HOTKEY_SECTION, HotkeyStr.at(i), hotkeys[i], nullptr, true);
    }

    for (size_t i = 0; i < GRAPHICS_CONFIG_COUNT; i++)
    {
        ini.SetLongValue(GRAPHICS_SECTION, GraphicsStr.at(i), graphics[i]);
    }

    return ini.SaveFile(path) == SI_OK && core_options.Save();
}

bool Config::Load(const char *path)
{
    LogFunctionName;
    LogDebug("path: %s", path);

    CSimpleIniA ini;
    if (ini.LoadFile(path) != SI_OK)
    {
        return false;
    }

    const char *tmp = ini.GetValue(MAIN_SECTION, "language", "ENGLISH");
    for (size_t i = 0; i < LANGUAGE_COUNT; i++)
    {
        if (strcmp(tmp, gLanguageNames[i]) == 0)
        {
            language = (LANGUAGE)i;
            break;
        }
    }

    rewind = ini.GetLongValue(MAIN_SECTION, "rewind");
    rewind_buf_size = ini.GetLongValue(MAIN_SECTION, "rewind_buf_size");
    tmp = ini.GetValue(MAIN_SECTION, "last_rom");
    if (tmp && File::Exist(tmp))
    {
        last_rom = tmp;
    }
    else
    {
        last_rom = "ux0:";
    }

    if (rewind_buf_size < MIN_REWIND_BUF_SIZE || rewind_buf_size > MAX_REWIND_BUF_SIZE)
    {
        rewind_buf_size = DEFAULT_REWIND_BUF_SIZE;
    }

    for (auto &control : control_maps)
    {
        control.retro = ini.GetLongValue(PsvKeyStr.at(control.psv), "retro");
        control.turbo = ini.GetBoolValue(PsvKeyStr.at(control.psv), "turbo");
    }

    for (size_t i = 0; i < HOT_KEY_COUNT; i++)
    {
        hotkeys[i] = ini.GetLongValue(HOTKEY_SECTION, HotkeyStr.at(i));
    }

    for (size_t i = 0; i < GRAPHICS_CONFIG_COUNT; i++)
    {
        graphics[i] = ini.GetLongValue(GRAPHICS_SECTION, GraphicsStr.at(i));
    }

    LogDebug("load end");
    return true;
}

retro_language Config::GetRetroLanguage()
{
    switch (language)
    {
    case LANGUAGE_CHINESE:
        return RETRO_LANGUAGE_CHINESE_SIMPLIFIED;
    case LANGUAGE_ENGLISH:
    default:
        return RETRO_LANGUAGE_ENGLISH;
    }
}