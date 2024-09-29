#include <SimpleIni.h>
#include "cheat.h"
#include "log.h"

#define KEY_BUF_SIZE 32
#define KEY(K) snprintf(key, KEY_BUF_SIZE, "cheat%d_" K, index);

#define GET_VALUE(K) \
    KEY(#K);         \
    this->K = ini->GetValue("", key, "");

#define GET_LONG_VALUE(K) \
    KEY(#K);              \
    this->K = ini->GetLongValue("", key, 0);

#define GET_BOOL_VALUE(K) \
    KEY(#K);              \
    this->K = ini->GetBoolValue("", key, false);

bool Cheat::Load(CSimpleIniA *ini, size_t index)
{
    char key[KEY_BUF_SIZE];
    GET_VALUE(desc);
    if (desc.size() == 0)
    {
        return false;
    }

    GET_VALUE(code);
    GET_BOOL_VALUE(enable);
    GET_BOOL_VALUE(big_endian);
    GET_LONG_VALUE(cheat_type);
    GET_LONG_VALUE(address);
    GET_LONG_VALUE(value);
    GET_LONG_VALUE(address_bit_position);
    GET_LONG_VALUE(repeat_count);
    GET_LONG_VALUE(repeat_add_to_address);
    GET_LONG_VALUE(repeat_add_to_value);
    GET_LONG_VALUE(memory_search_size);

    return true;
}

Cheats::Cheats()
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
        return false;
    }

    int size = ini.GetLongValue("", "cheats");
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
            LogDebug("cheat %d %s", count, cheat.desc.c_str());
            this->emplace_back(cheat);
        }
        else
        {
            break;
        }

        count++;
    }

    return true;
}
