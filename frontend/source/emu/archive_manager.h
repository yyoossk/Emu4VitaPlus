#pragma once
#include <stdint.h>
#include "cache_manager.h"

class ArchiveManager : public CacheManager
{
public:
    ArchiveManager();
    virtual ~ArchiveManager();

    const char *GetCachedPath(uint32_t crc32, const char *name, const char *entry_name);

private:
    bool _ExtractZip(uint32_t crc32, const char *name, const char *entry_name);
    bool _Extract7z(uint32_t crc32, const char *name, const char *entry_name);
    std::string _GetCachedName(uint32_t crc32, const char *entry_name);
    std::string _GetCachedFullName(uint32_t crc32, const char *entry_name);

    size_t _max_size;
};