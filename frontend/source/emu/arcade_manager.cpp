#ifdef ARC_BUILD
#include "arcade_manager.h"
#include "file.h"
#include "log.h"

ArcadeManager::ArcadeManager() : _names(nullptr)
{
    _Load();
}

ArcadeManager::~ArcadeManager()
{
    if (_names)
    {
        delete[] _names;
    }
}

void ArcadeManager::_Load(const char *path)
{
    LogFunctionName;
    uint32_t *buf;
    if (ReadFile(path void **buf) == 0)
    {
        LogError("failed to load %s", path);
        return;
    }

    uint32_t size = *buf;
    buf++;
    _names = new char[size];
    memcpy(_names, buf, size);
    buf += size / sizeof(uint32_t);

    uint32_t size = *buf;
    _name_hash.insert(buf, buf + size);

    delete[] buf;
}

#endif