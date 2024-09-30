#include <SimpleIni.h>
#include <stdlib.h>
#include "cheat.h"
#include "utils.h"
#include "log.h"
#include "emulator.h"
#include "app.h"

#define KEY_BUF_SIZE 32
#define KEY(K) snprintf(key, KEY_BUF_SIZE, "cheat%d_" K, index);

static std::string GetValue(CSimpleIniA *ini, const char *key)
{
    std::string s(ini->GetValue("", key, ""));
    StripQuotes(&s);
    return s;
}

static long GetLong(CSimpleIniA *ini, const char *key)
{
    std::string s = GetValue(ini, key);
    return strtol(s.c_str(), NULL, 10);
}

static std::string GetValue(CSimpleIniA *ini, const char *key, size_t index)
{
    char k[KEY_BUF_SIZE];
    snprintf(k, KEY_BUF_SIZE, "cheat%d_%s", index, key);
    return GetValue(ini, k);
}

static long GetLong(CSimpleIniA *ini, const char *key, size_t index)
{
    std::string s = GetValue(ini, key, index);
    return strtol(s.c_str(), NULL, 10);
}

static bool GetBool(CSimpleIniA *ini, const char *key, size_t index)
{
    std::string s = GetValue(ini, key, index);
    return s == "true";
}

#define GET_VALUE(K) K = GetValue(ini, #K, index)
#define GET_LONG(K) K = GetLong(ini, #K, index)
#define GET_BOOL(K) K = GetBool(ini, #K, index)

bool Cheat::Load(CSimpleIniA *ini, size_t index)
{
    char key[KEY_BUF_SIZE];
    GET_VALUE(desc);
    // LogDebug("desc %d %s", index, desc.c_str());
    if (desc.size() == 0)
    {
        return false;
    }

    GET_VALUE(code);
    GET_BOOL(enable);
    GET_BOOL(big_endian);
    GET_LONG(handler);
    GET_LONG(cheat_type);
    GET_LONG(address);
    GET_LONG(value);
    GET_LONG(address_bit_position);
    GET_LONG(repeat_count);
    GET_LONG(repeat_add_to_address);
    GET_LONG(repeat_add_to_value);
    GET_LONG(memory_search_size);

    return true;
}

void Cheat::Apply() const
{
    LogFunctionName;
}

Cheats::Cheats() : ThreadBase(_CheatThread)
{
}

Cheats::~Cheats()
{
}

bool Cheats::Load(const char *path)
{
    LogFunctionName;

    this->clear();

    CSimpleIniA ini;
    if (ini.LoadFile(path) != SI_OK)
    {
        LogDebug("load cheat failed: %s", path);
        return false;
    }

    int size = GetLong(&ini, "cheats");
    if (size > 0)
    {
        this->reserve(size);
    }

    int count = 0;
    while (true)
    {
        Cheat cheat;
        if (cheat.Load(&ini, count))
        {
            // LogDebug("  cheat %d %s", count, cheat.desc.c_str());
            this->emplace_back(cheat);
        }
        else
        {
            break;
        }

        count++;
    }

    return this->size() > 0;
}

int Cheats::_CheatThread(SceSize args, void *argp)
{
    LogFunctionName;

    CLASS_POINTER(Cheats, cheats, argp);

    while (gEmulator->GetFrameCount() < 300)
    {
        sceKernelDelayThread(20000);
    }

    while (gStatus.Get() & (APP_STATUS_RUN_GAME | APP_STATUS_SHOW_UI_IN_GAME) != 0)
    {
        bool applied = false;
        for (const Cheat &cheat : *cheats)
        {
            if (cheat.enable)
            {
                cheat.Apply();
                applied = true;
            }
        }

        if (!applied)
        {
            cheats->Wait();
        }
    }

    return 0;
}