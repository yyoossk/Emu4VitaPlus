#include "archive_manager.h"
#include "file.h"

#define ARCHIVE_CACHE_DIR CACHE_DIR "/archives"

ArchiveCache::ArchiveCache(size_t max_size)
{
    if (File::Exist(ARCHIVE_CACHE_DIR))
    {

        SceUID dfd = sceIoDopen(ARCHIVE_CACHE_DIR);
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