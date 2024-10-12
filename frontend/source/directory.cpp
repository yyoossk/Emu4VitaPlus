#include <psp2/io/dirent.h>
#include <algorithm>
#include <cctype>
#include <string.h>
#include <minizip/mz.h>
#include <minizip/mz_zip.h>
#include <minizip/mz_strm.h>
#include <minizip/mz_zip_rw.h>
#include <7z.h>
#include <7zFile.h>
#include <7zAlloc.h>
#include <7zCrc.h>
#include <7zVersion.h>
#include "directory.h"
#include "utils.h"
#include "log.h"
#include "file.h"

static const size_t INPUT_7Z_BUF_SIZE = 1 << 18;

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
    _7z_buf = new uint8_t[INPUT_7Z_BUF_SIZE];
    CrcGenerateTable();

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
    delete[] _7z_buf;
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

bool Directory::_LeagleTestZip(const char *name, DirItem *item)
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
                break;
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

bool Directory::_LeagleTest7z(const char *name, DirItem *item)
{
    CSzArEx db;
    CFileInStream archiveStream;
    CLookToRead2 lookStream;
    ISzAlloc allocImp{SzAlloc, SzFree};
    uint16_t tmp[SCE_FIOS_PATH_MAX];

    if (InFile_Open(&archiveStream.file, (_current_path + "/" + name).c_str()) != 0)
    {
        LogError("failed to open %s/%s", _current_path.c_str(), name);
        return false;
    }

    FileInStream_CreateVTable(&archiveStream);
    archiveStream.wres = 0;

    LookToRead2_CreateVTable(&lookStream, False);
    lookStream.buf = _7z_buf;
    lookStream.bufSize = INPUT_7Z_BUF_SIZE;
    lookStream.realStream = &archiveStream.vt;
    LookToRead2_INIT(&lookStream);

    SzArEx_Init(&db);

    bool result = (SzArEx_Open(&db, &lookStream.vt, &allocImp, &allocImp) == SZ_OK);
    if (!result)
    {
        LogError("SzArEx_Open failed: %s/%s", _current_path.c_str(), name);
        goto END;
    }

    result = false;
    for (uint32_t i = 0; (!result) && i < db.NumFiles; i++)
    {
        if (SzArEx_IsDir(&db, i))
        {
            continue;
        }

        SzArEx_GetFileNameUtf16(&db, i, tmp);
        std::string n = Utils::Utf16leToUtf8(tmp);
        result = _LeagleTest(n.c_str());
        if (result)
        {
            item->entry_name = n;
            item->crc32 = db.CRCs.Vals[i];
        }
    }

END:
    SzArEx_Free(&db, &allocImp);
    File_Close(&archiveStream.file);
    return result;
}

bool Directory::_LeagleTest(const char *name, DirItem *item)
{
    std::string ext = File::GetExt(name);

    if (ext.size() == 0)
    {
        return false;
    }

    if (_ext_filters.find(ext) != _ext_filters.end())
    {
        return true;
    }

    if (!item)
    {
        return false;
    }

    bool result = false;
    if (ext == "zip")
    {
        result = _LeagleTestZip(name, item);
    }
    else if (ext == "7z")
    {
        result = _LeagleTest7z(name, item);
    }

    return result;
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