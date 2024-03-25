#include <psp2/io/dirent.h>
#include "directory.h"

Directory::Directory(const char *path)
{
    if (path)
    {
        Load(path);
    }
}

Directory::~Directory()
{
}

bool Directory::Load(const char *path)
{
    SceUID dfd = sceIoDopen(path);
    if (dfd < 0)
    {
        return false;
    }
}