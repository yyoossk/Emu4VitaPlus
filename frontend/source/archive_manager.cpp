#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/rtc.h>
#include <minizip/mz.h>
#include <minizip/mz_zip.h>
#include <minizip/mz_strm.h>
#include <minizip/mz_zip_rw.h>
#include "archive_manager.h"
#include "file.h"
#include "defines.h"
#include "log.h"

ArchiveManager::ArchiveManager(size_t max_size)
{
    LogFunctionName;
    if (File::Exist(ARCHIVE_CACHE_DIR))
    {
        SceUID dfd = sceIoDopen(ARCHIVE_CACHE_DIR);
        SceIoDirent dir;
        while (sceIoDread(dfd, &dir) > 0)
        {
            if (SCE_S_ISREG(dir.d_stat.st_mode))
            {
                char *p;
                uint32_t crc = strtoul(dir.d_name, &p, 16);
                if (*p == '.')
                {
                    time_t time;
                    std::string name = std::string(ARCHIVE_CACHE_DIR) + "/" + dir.d_name;
                    File::GetCreateTime(name.c_str(), &time);
                    _cache[crc] = {time, name};
                    LogDebug("  cached: %s %08x %u", name.c_str(), crc, time);
                }
            }
        }
        sceIoDclose(dfd);
        _CheckSize();
    }
    else
    {
        File::MakeDirs(ARCHIVE_CACHE_DIR);
    }
}

ArchiveManager::~ArchiveManager()
{
}

const char *ArchiveManager::GetCachedPath(uint32_t crc32, const char *name, const char *entry_name)
{
    LogFunctionName;

    auto iter = _cache.find(crc32);
    if (iter != _cache.end())
    {
        LogDebug("  return: %s", iter->second.name.c_str());
        return iter->second.name.c_str();
    }

    if (!name || !entry_name)
    {
        return nullptr;
    }

    void *handle = mz_zip_reader_create();
    if (mz_zip_reader_open_file(handle, name) == MZ_OK)
    {
        int result = mz_zip_reader_locate_entry(handle, entry_name, true);
        char p[255];
        snprintf(p, 255, "%s/%08x%s", ARCHIVE_CACHE_DIR, crc32, strrchr(entry_name, '.'));
        result = mz_zip_reader_entry_save_file(handle, p);
        mz_zip_reader_close(handle);

        LogDebug(" extract: %s", p);
        _Set(crc32, p);
    }
    mz_zip_reader_delete(&handle);

    return GetCachedPath(crc32);
}

void ArchiveManager::_CheckSize()
{
    LogFunctionName;
    if (_cache.size() > _max_size)
    {
        uint32_t earliest_crc32 = _cache.begin()->first;
        time_t earliest_time = _cache.begin()->second.time;
        for (const auto &iter : _cache)
        {
            if (iter.second.time < earliest_time)
            {
                earliest_crc32 = iter.first;
                earliest_time = iter.second.time;
            }
        }

        sceIoRemove(_cache[earliest_crc32].name.c_str());
        _cache.erase(earliest_crc32);
    }
}

void ArchiveManager::_Set(uint32_t crc32, const std::string &name, SceDateTime sdtime)
{
    LogFunctionName;
    if (sdtime.year == 0)
    {
        sceRtcGetCurrentClockLocalTime(&sdtime);
    }

    time_t time;
    sceRtcGetTime_t(&sdtime, &time);
    _cache[crc32] = {time, name};
}