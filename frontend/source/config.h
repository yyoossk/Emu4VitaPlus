#pragma once
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "libretro.h"
#include "core_options.h"
#include "config_types.h"
#include "language_define.h"
#include "defines.h"
#include "input_descriptor.h"

#define MIN_REWIND_BUF_SIZE 5
#define MAX_REWIND_BUF_SIZE 50
#define REWIND_BUF_SIZE_STEP 5

namespace Emu4Vita
{
    class Config
    {
    public:
        Config();
        virtual ~Config();

        bool Load(const char *path = APP_CONFIG_PATH);
        bool Save(const char *path = APP_CONFIG_PATH);

        void Default();
        void DefaultControlMap();
        void DefaultHotKey();
        void DefaultGraphics();
        retro_language GetRetroLanguage();

        bool boot_from_arch;
        std::string last_rom;
        uint32_t language;
        uint32_t rewind;
        uint32_t rewind_buf_size;
        std::vector<ControlMapConfig> control_maps;
        uint32_t graphics[GRAPHICS_CONFIG_COUNT];
        uint32_t hotkeys[HOT_KEY_COUNT];
        CoreOptions core_options;
        InputDescriptors input_descriptors;
        bool mute;

        static const std::unordered_map<uint32_t, const char *> PsvKeyStr;
        static const std::unordered_map<uint32_t, const char *> HotkeyStr;
        static const std::unordered_map<uint32_t, LanguageString> ControlTextMap;
        static const std::unordered_map<uint32_t, const char *> GraphicsStr;
        static const std::unordered_map<uint8_t, LanguageString> RetroTextMap;

    private:
    };
}

extern Emu4Vita::Config *gConfig;