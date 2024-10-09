#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/rtc.h>
#include <minizip/mz.h>
#include <minizip/mz_zip.h>
#include <minizip/mz_strm.h>
#include <minizip/mz_zip_rw.h>
#include <7z.h>
#include <7zFile.h>
#include <7zAlloc.h>
#include <7zCrc.h>
#include "archive_manager.h"
#include "file.h"
#include "utils.h"
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

    std::string ext = File::GetExt(name);
    if (ext == "zip")
    {
        if (!_ExtractZip(crc32, name, entry_name))
        {
            return nullptr;
        }
    }
    else if (ext == "7z")
    {
        if (!_Extract7z(crc32, name, entry_name))
        {
            return nullptr;
        }
    }

    _CheckSize();
    return GetCachedPath(crc32);
}

bool ArchiveManager::_ExtractZip(uint32_t crc32, const char *name, const char *entry_name)
{
    LogFunctionName;

    void *handle = mz_zip_reader_create();
    int result = mz_zip_reader_open_file(handle, name);
    if (result != MZ_OK)
    {
        goto ZIP_END;
    }

    result = mz_zip_reader_locate_entry(handle, entry_name, true);
    if (result)
    {
        char p[SCE_FIOS_PATH_MAX];
        snprintf(p, SCE_FIOS_PATH_MAX, "%s/%08x%s", ARCHIVE_CACHE_DIR, crc32, strrchr(entry_name, '.'));
        result = mz_zip_reader_entry_save_file(handle, p);
        if (result)
        {
            LogDebug(" extract: %s", p);
            _Set(crc32, p);
        }
    }

    mz_zip_reader_close(handle);

ZIP_END:
    mz_zip_reader_delete(&handle);

    return result == MZ_OK;
}

bool ArchiveManager::_Extract7z(uint32_t crc32, const char *name, const char *entry_name)
{
    LogFunctionName;

    CSzArEx db;
    CFileInStream archiveStream;
    CLookToRead2 lookStream;
    ISzAlloc allocImp{SzAlloc, SzFree};
    uint16_t tmp[SCE_FIOS_PATH_MAX];

    if (InFile_Open(&archiveStream.file, name) != 0)
    {
        LogError("failed to open %s", name);
        return false;
    }

    FileInStream_CreateVTable(&archiveStream);
    archiveStream.wres = 0;

    LookToRead2_CreateVTable(&lookStream, False);
    lookStream.buf = new uint8_t[1 << 18];
    lookStream.bufSize = 1 << 18;
    lookStream.realStream = &archiveStream.vt;
    LookToRead2_INIT(&lookStream);
    SzArEx_Init(&db);

    bool result = (SzArEx_Open(&db, &lookStream.vt, &allocImp, &allocImp) == SZ_OK);
    if (!result)
    {
        LogError("SzArEx_Open failed: %s", name);
        goto END;
    }

    result = false;
    for (uint32_t i = 0; i < db.NumFiles; i++)
    {
        if (SzArEx_IsDir(&db, i) || db.CRCs.Vals[i] != crc32)
        {
            continue;
        }

        uint8_t *buf = nullptr;
        size_t buf_size = 0;
        uint32_t block_index = 0xffffffff;
        size_t offset = 0;
        size_t out_size = 0;
        result = (SzArEx_Extract(&db, &lookStream.vt, i,
                                 &block_index, &buf, &buf_size,
                                 &offset, &out_size,
                                 &allocImp, &allocImp) == SZ_OK);
        if (result)
        {
            char p[SCE_FIOS_PATH_MAX];
            snprintf(p, SCE_FIOS_PATH_MAX, "%s/%08x%s", ARCHIVE_CACHE_DIR, crc32, strrchr(entry_name, '.'));
            result = File::WriteFile(p, buf, buf_size);
            if (result)
            {
                LogDebug(" extract: %s", p);
                _Set(crc32, p);
            }
        }
        ISzAlloc_Free(&allocImp, buf);
        break;
    }

END:
    delete[] lookStream.buf;
    SzArEx_Free(&db, &allocImp);
    File_Close(&archiveStream.file);

    return result;
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