#pragma once
#include <string>
#include <unordered_map>
#include <stdint.h>
#include <time>

#define DEFAULT_CACHE_SIZE 10

struct CachedItem
{
    time_t time;
    std::string name;
};

class ArchiveCache
{
public:
    ArchiveCache(size_t max_size);
    virtual ~ArchiveCache();

    const CachedItem *Find(uint32_t crc32);
    void Set(uint32_t crc32, SceDateTime sdtime, const std::string &name);
    void Set(uint32_t crc32, time_t time, const std::string &name);

private:
    void _CheckSize();

    std::unordered_map<uint32_t, CachedItem> _cache;
    size_t _max_size;
};

class ArchiveManager
{
public:
    ArchiveManager();
    virtual ~ArchiveManager();

    const char *GetCachedPath(const char *name, int index);
    size_t GetCachedMemory(const char *name, int index, void **buf);

private:
    ArchiveCache _cache(DEFAULT_CACHE_SIZE);
};