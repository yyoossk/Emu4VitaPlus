#include <psp2/io/fcntl.h>
#include <string.h>
#include "file.h"

File::File(const char *name)
{
}

File::~File()
{
}

bool File::Exist(const char *name)
{
    SceIoStat stat;
    return sceIoGetstat(name, &stat) == SCE_OK;
}

void File::MakeDirs(const char *path, SceIoMode mode)
{
    if (!(path && *path) || Exist(path))
    {
        return;
    }

    char *_path = new char[strlen(path) + 1];
    strcpy(_path, path);
    char *p = _path;
    do
    {
        p = strchr(p, '/');
        if (p)
        {
            *p = '\0';
        }

        if (!Exist(_path))
        {
            sceIoMkdir(_path, mode);
        }

        if (p)
        {
            *p = '/';
            p++;
        }
        else
        {
            break;
        }
    } while (true);

    delete[] _path;
}

bool File::Remove(const char *path)
{
    return sceIoRemove(path) == SCE_OK;
}