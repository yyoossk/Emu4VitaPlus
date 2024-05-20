#include <psp2/io/fcntl.h>
#include <psp2/rtc.h>
#include <string.h>
#include "file.h"
#include "log.h"

namespace File
{
    bool Exist(const char *name)
    {
        SceIoStat stat;
        return sceIoGetstat(name, &stat) == SCE_OK;
    }

    bool GetCreateTime(const char *name, SceDateTime *time)
    {
        SceIoStat stat;
        if (sceIoGetstat(name, &stat) != SCE_OK)
        {
            LogWarn("failed to get stat: %s", name);
            return false;
        }

        SceRtcTick tick;
        sceRtcGetTick(&stat.st_mtime, &tick);
        sceRtcConvertUtcToLocalTime(&tick, &tick);
        sceRtcSetTick(time, &tick);

        return true;
    }

    void
    MakeDirs(const char *path, SceIoMode mode)
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

    bool Remove(const char *path)
    {
        return sceIoRemove(path) == SCE_OK;
    }

    std::string GetStem(const char *path)
    {
        const char *start = strrchr(path, '/');
        if (start == NULL)
        {
            start = strrchr(path, ':');
        }

        if (start == NULL)
        {
            start = path;
        }
        else
        {
            start++;
        }

        const char *end = strrchr(path, '.');
        if (end == NULL)
        {
            end = path + strlen(path);
        }

        return std::string(start, end - start);
    }
}