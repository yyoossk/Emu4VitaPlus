#pragma once
#include <string>
#include <unordered_map>
#include <stdint.h>
#include <time.h>

#define DEFAULT_CACHE_SIZE 10

struct CachedItem
{
    time_t time;
    std::string name;
};

class ArchiveManager
{
public:
    ArchiveManager(size_t max_size);
    virtual ~ArchiveManager();

    const char *GetCachedPath(uint32_t crc32, const char *name = nullptr, const char *entry_name = nullptr);
    size_t GetCachedMemory(const char *name, const char *entry_name, uint32_t crc32, void **buf);

private:
    void _Set(uint32_t crc32, const std::string &name, SceDateTime sdtime = {0});
    void _CheckSize();
    bool _ExtractZip(uint32_t crc32, const char *name, const char *entry_name);
    bool _Extract7z(uint32_t crc32, const char *name, const char *entry_name);

    std::unordered_map<uint32_t, CachedItem> _cache;
    size_t _max_size;
};