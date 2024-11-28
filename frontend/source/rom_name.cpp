#include "rom_name.h"
#include "file.h"
#include "config.h"
#include "language_string.h"
#include "log.h"
#include "defines.h"

RomNameMap::RomNameMap(const char *path) : _names(nullptr)
{
    if (path != nullptr)
    {
        Load(path);
    }
}

RomNameMap::~RomNameMap()
{
    if (_names != nullptr)
        delete[] _names;
}

bool RomNameMap::Load(const char *path)
{
    LogFunctionName;
    _map.clear();
    if (_names != nullptr)
    {
        delete[] _names;
        _names = nullptr;
    }

    char *buf;
    if (File::ReadCompressedFile(path, (void **)&buf) == 0)
    {
        LogError("failed to load %s", path);
        return false;
    }

    uint32_t *p = (uint32_t *)buf;

    uint32_t size = *p++;
    _map.reserve(size);
    for (size_t i = 0; i < size; i++)
    {
        uint32_t key = *p++;
        uint32_t value = *p++;
        _map.emplace(key, value);
    }

    size = *p++;
    _names = new char[size];
    memcpy(_names, p, size);

    delete[] buf;

    LogDebug("  Load %d names from %s", _map.size(), path);
    LogDebug("  name buf size: %d", size);

    return true;
}

bool RomNameMap::Load(const std::string &path)
{
    return Load(path.c_str());
}

const char *RomNameMap::GetName(uint32_t crc) const
{
    LogFunctionName;

    if (_names == nullptr)
        return nullptr;

    LogDebug("%08x", crc);
    const auto &iter = _map.find(crc);
    if (iter == _map.end())
    {
        return nullptr;
    }
    LogDebug("%08x %08x", _names, iter->second);
    return _names + iter->second;
}

// const char *RomNameMap::GetName(const char *full_path, uint32_t crc) const
// {
//     if (IS_ARCADE)
//     {
//     }
// }

void RomNameMap::Load()
{
    bool result = Load(std::string("app0:assets/names.") + TEXT(CODE) + ".zdb");
    if (!result && gConfig->language != LANGUAGE_ENGLISH)
    {
        Load("app0:assets/names.en.zdb");
    }
}