#include <psp2/io/dirent.h>
#include <string.h>
#include "directory.h"
#include "log.h"

Directory::Directory(const char *path, const char *ext_filters, char split)
{
    LogFunctionName;

    if (ext_filters)
    {
        _SetExtensionFilter(ext_filters, split);
    }

    if (path)
    {
        SetCurrentPath(path);
    }
}

Directory::~Directory()
{
    LogFunctionName;
}

void Directory::_SetExtensionFilter(const char *exts, char split)
{
    LogFunctionName;

    char *exts_string = new char[strlen(exts) + 1];
    strcpy(exts_string, exts);
    char *p = exts_string;
    char *end;
    do
    {
        end = strchr(p, split);
        if (end)
        {
            *end = '\x00';
        }
        _ext_filters.insert(p);

        if (end)
        {
            end++;
            p = end;
        }
        else
        {
            break;
        }
    } while (true);

    delete[] exts_string;
}

bool Directory::SetCurrentPath(const std::string &path)
{
    LogFunctionName;

    _items.clear();

    SceUID dfd = sceIoDopen(path.c_str());
    if (dfd < 0)
    {
        return false;
    }

    _current_path = path;
    std::vector<DirItem> files;

    SceIoDirent dir;
    while (sceIoDread(dfd, &dir) > 0)
    {
        if (SCE_S_ISDIR(dir.d_stat.st_mode))
        {
            _items.push_back({dir.d_name, true});
        }
        else
        {
            files.push_back({dir.d_name, false});
        }
    }
    sceIoDclose(dfd);

    _items.insert(_items.end(), files.begin(), files.end());

    return true;
}

int Directory::GetSize()
{
    return _items.size();
}