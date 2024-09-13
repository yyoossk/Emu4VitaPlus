#include <psp2/io/dirent.h>
#include <algorithm>
#include <cctype>
#include <string.h>
#include <minizip/mz.h>
#include <minizip/mz_zip.h>
#include <minizip/mz_strm.h>
#include <minizip/mz_zip_rw.h>
#include "directory.h"
#include "utils.h"
#include "log.h"
#include "file.h"

std::unordered_set<std::string> Directory::_ext_archives{"zip"}; //, "7z"};

static void SortDirItemsByNameIgnoreCase(std::vector<DirItem> &items)
{
    std::sort(items.begin(), items.end(), [](const DirItem &a, const DirItem &b)
              { return std::lexicographical_compare(
                    a.name.begin(), a.name.end(), b.name.begin(), b.name.end(),
                    [](unsigned char ch1, unsigned char ch2)
                    { return std::tolower(ch1) < std::tolower(ch2); }); });
}

Directory::Directory(const char *path, const char *ext_filters, char split)
{
    LogFunctionName;

    _zip_handle = mz_zip_reader_create();

    if (ext_filters)
    {
        SetExtensionFilter(ext_filters, split);
    }

    if (path)
    {
        SetCurrentPath(path);
    }
}

Directory::~Directory()
{
    LogFunctionName;
    mz_zip_reader_delete(&_zip_handle);
}

void Directory::SetExtensionFilter(const char *exts, char split)
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

bool Directory::_LeagleTest(const char *name, DirItem *item)
{
    char *ext = strrchr(name, '.');
    if (!ext)
    {
        return false;
    }

    ext++;

    std::string n(ext);
    Lower(&n);

    if (_ext_filters.find(n) != _ext_filters.end())
    {
        return true;
    }

    if (!item)
    {
        return false;
    }

    if (_ext_archives.find(n) != _ext_archives.end())
    {
        bool result = false;
        if (mz_zip_reader_open_file(_zip_handle, (_current_path + "/" + name).c_str()) == MZ_OK)
        {
            mz_zip_reader_goto_first_entry(_zip_handle);
            do
            {
                mz_zip_file *info;
                mz_zip_reader_entry_get_info(_zip_handle, &info);
                result = _LeagleTest(info->filename);
                if (result)
                {
                    item->entry_name = info->filename;
                    item->crc32 = info->crc;
                }
            } while ((!result) && mz_zip_reader_goto_next_entry(_zip_handle) == MZ_OK);

            mz_zip_reader_close(_zip_handle);
        }
        else
        {
            LogError("failed to open %s/%s", _current_path.c_str(), name);
        }

        return result;
    }

    return false;
}

bool Directory::SetCurrentPath(const std::string &path)
{
    LogFunctionName;
    LogDebug("  path: %s", path.c_str());

    if (path.size() == 0)
    {
        return _ToRoot();
    }

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
        if (*dir.d_name == '.')
        {
            continue;
        }
        else if (SCE_S_ISDIR(dir.d_stat.st_mode))
        {
            _items.push_back({dir.d_name, true});
        }
        else
        {
            DirItem item{dir.d_name, false};
            if (_LeagleTest(dir.d_name, &item))
            {
                files.push_back(item);
            }
        }
    }
    sceIoDclose(dfd);

    SortDirItemsByNameIgnoreCase(_items);
    SortDirItemsByNameIgnoreCase(files);

    _items.insert(_items.end(), files.begin(), files.end());

    return true;
}

size_t Directory::GetSize()
{
    return _items.size();
}

bool Directory::_ToRoot()
{
    _items.clear();
    _current_path = "";

    for (const auto device : {
             "imc0:",
             "uma0:",
             "ur0:",
             "ux0:",
             "xmc0:",
         })
    {
        if (File::Exist(device))
        {
            _items.push_back({device, true});
        }
    }

    return _items.size() > 0;
}