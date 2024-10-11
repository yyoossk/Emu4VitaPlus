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

#define DEFAULT_CACHE_SIZE 10

ArchiveManager::ArchiveManager() : CacheManager(ARCHIVE_CACHE_DIR, DEFAULT_CACHE_SIZE)
{
}

ArchiveManager::~ArchiveManager()
{
}

const char *ArchiveManager::GetCachedPath(uint32_t crc32, const char *name, const char *entry_name)
{
    LogFunctionName;
    static std::string full_path;
    if (IsInCache(_GetCachedName(crc32, entry_name).c_str()))
    {
        full_path = _GetCachedFullName(crc32, entry_name);
        LogDebug("  return: %s", full_path.c_str());
        return full_path.c_str();
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

    full_path = _GetCachedFullName(crc32, entry_name);
    LogDebug("  return: %s", full_path.c_str());
    return full_path.c_str();
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
        result = mz_zip_reader_entry_save_file(handle, _GetCachedFullName(crc32, entry_name).c_str());
        LogDebug("  extract %s", _GetCachedFullName(crc32, entry_name).c_str());
        if (result)
        {
            Set(_GetCachedName(crc32, entry_name).c_str());
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
            result = File::WriteFile(_GetCachedFullName(crc32, entry_name).c_str(), buf, buf_size);
            if (result)
            {
                LogDebug("  extract %s", _GetCachedFullName(crc32, entry_name).c_str());
                Set(_GetCachedName(crc32, entry_name).c_str());
            }
        }
        else
        {
            LogError("Extract failed");
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

std::string ArchiveManager::_GetCachedName(uint32_t crc32, const char *entry_name)
{
    char path[SCE_FIOS_PATH_MAX];
    snprintf(path, SCE_FIOS_PATH_MAX, "%08x%s", crc32, strrchr(entry_name, '.'));
    return std::string(path);
}

std::string ArchiveManager::_GetCachedFullName(uint32_t crc32, const char *entry_name)
{
    return GetDirPath() + "/" + _GetCachedName(crc32, entry_name);
}