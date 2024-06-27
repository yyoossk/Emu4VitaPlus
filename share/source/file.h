#pragma once
#include <psp2/io/stat.h>
#include <string>

#define SCE_FIOS_PATH_MAX 1024

namespace File
{

    bool Exist(const char *path);
    bool GetCreateTime(const char *name, SceDateTime *time);
    bool GetCreateTime(const char *name, time_t *time);
    size_t GetSize(const char *name);
    bool ReadFile(const char *name, void *buf, SceSSize size);
    void MakeDirs(const char *path, SceIoMode mode = (SceIoMode)0777);
    bool Remove(const char *path);
    std::string GetStem(const char *path);
}