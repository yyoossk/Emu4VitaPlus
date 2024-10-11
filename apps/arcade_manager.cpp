#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/rtc.h>
#include "arcade_manager.h"
#include "file.h"
#include "log.h"
#include "emulator.h"

#define ARC_CACHE_MAX_SIZE 10

void Emulator::_InitArcadeManager()
{
#ifdef ARC_BUILD
    _arcade_manager = new ArcadeManager();
#endif
}

ArcadeManager::ArcadeManager() : CacheManager(ARCADE_CACHE_DIR, ARC_CACHE_MAX_SIZE), _names(nullptr)
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

void ArcadeManager::_Load()
{
    LogFunctionName;
    uint32_t *buf;
    if (File::ReadFile(DAT_BIN_PATH, (void **)&buf) == 0)
    {
        LogError("failed to load %s", DAT_BIN_PATH);
        return;
    }

    uint32_t size = *buf++;
    _names = new char[size];
    memcpy(_names, buf, size);
    buf += size / sizeof(uint32_t);

    size = *buf++;
    _name_hash.insert(buf, buf + size);
    buf += size;

    size = *buf++;

    for (size_t i = 0; i < size; i++)
    {
        uint32_t n = *buf++;
        uint32_t crc32 = *buf++;
        _roms[crc32] = std::unordered_set<uint32_t>{buf, buf + n};
        buf += n;
    }

    LogDebug("%08x %08x", _name_hash.size(), _roms.size());

    delete buf;
}
