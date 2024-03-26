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
        _extFilters.insert(p);

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

bool Directory::SetCurrentPath(const char *path)
{
    LogFunctionName;

    _items.clear();

    SceUID dfd = sceIoDopen(path);
    if (dfd < 0)
    {
        return false;
    }

    _currentPath = path;
    std::vector<std::string> files;

    SceIoDirent dir;
    while (sceIoDread(dfd, &dir) > 0)
    {
        if (SCE_S_ISDIR(dir.d_stat.st_mode))
        {
            _items.emplace_back(std::string(dir.d_name) + '/');
        }
        else
        {
            files.emplace_back(dir.d_name);
        }
    }
    sceIoDclose(dfd);

    _items.insert(_items.end(), files.begin(), files.end());

    return true;
}

const char *Directory::GetItem(int index)
{
    if (index >= 0 && index < (int)_items.size())
        return _items[index].c_str();
    else
        return nullptr;
}

int Directory::GetSize()
{
    return _items.size();
}