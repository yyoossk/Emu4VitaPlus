#pragma once
#include <psp2/io/stat.h>

class File
{
public:
    File(const char *name);
    virtual ~File();

    static bool Exist(const char *path);
    static void MakeDirs(const char *path, SceIoMode mode = (SceIoMode)0777);

private:
};