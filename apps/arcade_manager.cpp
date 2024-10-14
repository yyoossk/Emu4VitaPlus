#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/rtc.h>
#include <zlib.h>
#include "arcade_manager.h"
#include "file.h"
#include "log.h"
#include "emulator.h"
#include "archive_reader_factory.h"
#include "file.h"

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
    uint32_t *p = buf;

    uint32_t size = *p++;
    _names = new char[size];
    memcpy(_names, p, size);
    p += size / sizeof(uint32_t);

    size = *p++;
    _name_hash.insert(p, p + size);
    p += size;

    size = *p++;

    for (size_t i = 0; i < size; i++)
    {
        uint32_t n = *p++;
        uint32_t crc32 = *p++;
        _roms[crc32] = std::unordered_set<uint32_t>{p, p + n};
        p += n;
    }

    delete[] buf;
}

const char *ArcadeManager::GetCachedPath(const char *path)
{
    LogFunctionName;
    static char cached_path[SCE_FIOS_PATH_MAX];

    std::string stem = File::GetStem(path);

    if (_name_hash.find(crc32(0, (Bytef *)stem.c_str(), stem.size())) != _name_hash.end())
    {
        LogDebug("  %s found.", stem.c_str());
        return path;
    }

    ArchiveReader *reader = gArchiveReaderFactory->Get(path);
    if (reader == nullptr || !reader->Open(path))
    {
        return nullptr;
    }

    std::unordered_map<uint32_t, uint32_t> offsets; // offset:count
    do
    {
        uint32_t crc32 = reader->GetCurrentCrc32();
        auto iter = _roms.find(crc32);
        if (iter == _roms.end())
        {
            continue;
        }

        for (const auto &offset : iter->second)
        {
            if (offsets.find(offset) == offsets.end())
            {
                offsets[offset] = 1;
            }
            else
            {
                offsets[offset] += 1;
            }
        }
    } while (reader->Next());

    if (offsets.size() == 0)
    {
        return nullptr;
    }

    uint32_t offset = 0;
    uint32_t max_count = 0;
    for (const auto &iter : offsets)
    {
        LogDebug("%08x %s %d", iter.first, _names + iter.first, iter.second);
        if (iter.second > max_count)
        {
            offset = iter.first;
            max_count = iter.second;
        }
    }

    snprintf(cached_path, SCE_FIOS_PATH_MAX, "%s/%s.%s", ARCADE_CACHE_DIR, _names + offset, File::GetExt(path).c_str());

    return File::CopyFile(path, cached_path) ? cached_path : nullptr;
}