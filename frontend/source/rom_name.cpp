#include <string.h>
#include "rom_name.h"
#include "file.h"
#include "log.h"

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
    if (File::ReadFile(path, (void **)&buf) == 0)
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
        _map[key] = value;
    }

    size = *p++;
    _names = new char[size];
    memcpy(_names, p, size);

    delete[] buf;

    return true;
}

const char *RomNameMap::GetName(uint32_t crc) const
{
    const auto &iter = _map.find(crc);
    if (iter == _map.end())
    {
        return nullptr;
    }
    return _names + iter->second;
}