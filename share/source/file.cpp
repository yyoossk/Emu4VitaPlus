#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h>
#include <psp2/rtc.h>
#include <string.h>
#include "file.h"
#include "log.h"
#include "utils.h"

#define SCE_STM_RWU (00600)
#define SCE_STM_RU (00400)

namespace File
{
    bool Exist(const char *name)
    {
        SceIoStat stat;
        return sceIoGetstat(name, &stat) == SCE_OK;
    }

    size_t GetSize(const char *name)
    {
        SceIoStat stat;
        if (sceIoGetstat(name, &stat) == SCE_OK)
        {
            return stat.st_size;
        }

        return 0;
    }

    bool ReadFile(const char *name, void *buf, SceSSize size)
    {
        SceUID fd = sceIoOpen(name, SCE_O_RDONLY, SCE_STM_RU);
        if (fd <= 0)
        {
            return false;
        }

        SceSSize s = sceIoRead(fd, buf, size);
        sceIoClose(fd);

        return s == size;
    }

    size_t ReadFile(const char *name, void **buf)
    {
        size_t size = GetSize(name);
        if (size == 0)
        {
            return 0;
        }

        *buf = new uint8_t[size];
        if (ReadFile(name, *buf, size))
        {
            return size;
        }
        else
        {
            delete[] (uint8_t *)*buf;
            *buf = nullptr;
            return 0;
        }
    }

    bool WriteFile(const char *name, void *buf, SceSSize size)
    {
        SceUID fd = sceIoOpen(name, SCE_O_CREAT | SCE_O_WRONLY, SCE_STM_RWU);
        if (fd <= 0)
        {
            return false;
        }
        SceSSize s = sceIoWrite(fd, buf, size);
        sceIoClose(fd);
        return s == size;
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

    bool GetCreateTime(const char *name, time_t *time)
    {
        SceDateTime t;
        if (GetCreateTime(name, &t))
        {
            sceRtcGetTime_t(&t, time);
            return true;
        }
        return false;
    }

    void MakeDirs(const char *path, SceIoMode mode)
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

    void RemoveAllFiles(const char *path)
    {
        SceUID dfd = sceIoDopen(path);
        if (dfd < 0)
        {
            return;
        }

        std::string p(path);
        p.push_back('/');

        SceIoDirent dir;
        while (sceIoDread(dfd, &dir) > 0)
        {
            if (*dir.d_name == '.')
            {
                continue;
            }
            else if (SCE_S_ISREG(dir.d_stat.st_mode))
            {
                sceIoRemove((p + dir.d_name).c_str());
            }
        }
        sceIoDclose(dfd);
    }

    static const char *_GetDirEndPos(const char *path)
    {
        return strrchr(path, '/');
    }

    static char *_GetDotPos(const char *path)
    {
        return strrchr(path, '.');
    }

    std::string GetName(const char *path)
    {
        const char *start = _GetDirEndPos(path);
        if (start)
        {
            start++;
            return start;
        }
        else
        {
            return path;
        }
    }

    std::string GetStem(const char *path)
    {
        const char *start = _GetDirEndPos(path);
        const char *end = _GetDotPos(path);

        if (!start)
            start = path;
        else
            start++;

        if (!end)
            end = path + strlen(path);

        return std::string(start, end - start);
    }

    std::string GetExt(const char *path, bool lower)
    {
        const char *dot = _GetDotPos(path);
        if (dot)
        {
            std::string s(dot + 1);
            if (lower)
            {
                Utils::Lower(&s);
            }
            return s;
        }
        else
        {
            return "";
        }
    }

    std::string GetDir(const char *path)
    {
        const char *end = _GetDirEndPos(path);
        return end ? std::string(path, end - path) : std::string(path);
    }
}
