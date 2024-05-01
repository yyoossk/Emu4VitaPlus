#pragma once
#include <psp2/io/stat.h>

namespace File
{

    bool Exist(const char *path);
    void MakeDirs(const char *path, SceIoMode mode = (SceIoMode)0777);
    bool Remove(const char *path);

    class File
    {
    public:
        File(const char *name);
        virtual ~File();

    private:
    };

}