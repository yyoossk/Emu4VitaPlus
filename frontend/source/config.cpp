#include <psp2/ctrl.h>
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
#define CORE_SECTION "CORE"

Config *gConfig = nullptr;

#define KEY_PAIR(K) \
    {               \
        K, #K       \
    }

static const std::unordered_map<uint32_t, const char *> PsvKeyStr = {
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

static const std::unordered_map<uint32_t, const char *> HotkeyStr = {
    KEY_PAIR(SAVE_STATE),
    KEY_PAIR(LOAD_STATE),
    KEY_PAIR(GAME_SPEED_UP),
    KEY_PAIR(GAME_SPEED_DOWN),
    KEY_PAIR(GAME_REWIND),
    KEY_PAIR(CONTROLLER_PORT_UP),
    KEY_PAIR(CONTROLLER_PORT_DOWN),
    KEY_PAIR(EXIT_GAME),
};

static const std::unordered_map<uint32_t, const char *> GraphicsStr = {
    KEY_PAIR(DISPLAY_SIZE),
    KEY_PAIR(DISPLAY_RATIO),
    KEY_PAIR(DISPLAY_ROTATE),
    KEY_PAIR(GRAPHICS_SHADER),
    KEY_PAIR(GRAPHICS_SMOOTH),
    KEY_PAIR(GRAPHICS_OVERLAY),
};

std::unordered_map<uint32_t, TEXT_ENUM>
    Config::ControlTextMap = {
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

std::unordered_map<uint8_t, TEXT_ENUM> Config::RetroTextMap = {
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

Config::Config() : language(LANGUAGE_ENGLISH), hotkeys{0}
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
    DefaultControlMap();
    DefaultHotKey();
    DefaultGraphics();
}

void Config::DefaultControlMap()
{
    LogFunctionName;
    control_maps = {
#if defined(GBA_BUILD)
        {SCE_CTRL_UP, RETRO_DEVICE_ID_JOYPAD_UP},
        {SCE_CTRL_DOWN, RETRO_DEVICE_ID_JOYPAD_DOWN},
        {SCE_CTRL_LEFT, RETRO_DEVICE_ID_JOYPAD_LEFT},
        {SCE_CTRL_RIGHT, RETRO_DEVICE_ID_JOYPAD_RIGHT},
        {SCE_CTRL_CROSS, RETRO_DEVICE_ID_JOYPAD_B},
        {SCE_CTRL_TRIANGLE, RETRO_DEVICE_ID_JOYPAD_B, true},
        {SCE_CTRL_CIRCLE, RETRO_DEVICE_ID_JOYPAD_A},
        {SCE_CTRL_SQUARE, RETRO_DEVICE_ID_JOYPAD_A, true},
        {SCE_CTRL_SELECT, RETRO_DEVICE_ID_JOYPAD_SELECT},
        {SCE_CTRL_START, RETRO_DEVICE_ID_JOYPAD_START},
        {SCE_CTRL_L1, RETRO_DEVICE_ID_JOYPAD_L},
        {SCE_CTRL_R1, RETRO_DEVICE_ID_JOYPAD_R},
#endif
        {SCE_CTRL_L2},
        {SCE_CTRL_R2},
        {SCE_CTRL_L3},
        {SCE_CTRL_R3},
        {SCE_CTRL_LSTICK_UP, RETRO_DEVICE_ID_JOYPAD_UP},
        {SCE_CTRL_LSTICK_DOWN, RETRO_DEVICE_ID_JOYPAD_DOWN},
        {SCE_CTRL_LSTICK_LEFT, RETRO_DEVICE_ID_JOYPAD_LEFT},
        {SCE_CTRL_LSTICK_RIGHT, RETRO_DEVICE_ID_JOYPAD_RIGHT},
        {SCE_CTRL_RSTICK_UP},
        {SCE_CTRL_RSTICK_DOWN},
        {SCE_CTRL_RSTICK_LEFT},
        {SCE_CTRL_RSTICK_RIGHT},
    };
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
    graphics[GRAPHICS_SHADER] = CONFIG_GRAPHICS_SHADER_DEFAULT;
    graphics[GRAPHICS_SMOOTH] = CONFIG_GRAPHICS_SMOOTHER_NO;
    graphics[GRAPHICS_OVERLAY] = CONFIG_GRAPHICS_OVERLAY_MODE_OVERLAY;
}

bool Config::Save(const char *path)
{
    LogFunctionName;
    LogDebug("path: %s", path);

    CSimpleIniA ini;

    ini.SetValue(MAIN_SECTION, "language", gLanguageNames[language]);
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

    for (auto const &option : core_options)
    {
        ini.SetValue(CORE_SECTION, option.first.c_str(), option.second.value.c_str());
    }

    return ini.SaveFile(path) == SI_OK;
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

    const char *lang = ini.GetValue(MAIN_SECTION, "language", "ENGLISH");
    for (size_t i = 0; i < LANGUAGE_COUNT; i++)
    {
        if (strcmp(lang, gLanguageNames[i]) == 0)
        {
            language = (LANGUAGE)i;
            break;
        }
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

    CSimpleIniA::TNamesDepend keys;
    ini.GetAllKeys(CORE_SECTION, keys);
    for (const auto &iter : keys)
    {
        core_options[iter.pItem] = CoreOption();
        core_options[iter.pItem].value = ini.GetValue(CORE_SECTION, iter.pItem);
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