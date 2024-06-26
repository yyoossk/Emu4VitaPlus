#include <psp2/io/dirent.h>
#include <psp2/rtc.h>
#include "archive_manager.h"
#include "file.h"
#include "defines.h"

#define ARCHIVE_CACHE_DIR CACHE_DIR "/archives"

ArchiveCache::ArchiveCache(size_t max_size)
{
    if (File::Exist(ARCHIVE_CACHE_DIR))
    {
        SceUID dfd = sceIoDopen(ARCHIVE_CACHE_DIR);
        SceIoDirent dir;
        while (sceIoDread(dfd, &dir) > 0)
        {
            if (SCE_S_ISREG(dir.d_stat.st_mode))
            {
            }
        }
        sceIoDclose(dfd);
    }
    else
    {
        File::MakeDirs(ARCHIVE_CACHE_DIR);
    }
}

ArchiveCache::~ArchiveCache()
{
}