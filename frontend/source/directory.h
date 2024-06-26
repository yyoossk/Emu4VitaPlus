#pragma once
#include <vector>
#include <unordered_set>
#include <string>
#include <minizip/mz_zip.h>

struct DirItem
{
    std::string name;
    bool is_dir;
    int entry_index;
};

class Directory
{
public:
    Directory(const char *path = nullptr,
              const char *ext_filters = NULL,
              char split = '|');
    virtual ~Directory();

    void SetExtensionFilter(const char *exts, char split);
    bool SetCurrentPath(const std::string &path);
    const std::string &GetCurrentPath() const { return _current_path; };
    const DirItem &GetItem(int index) const { return _items[index]; };
    const std::string &GetItemName(int index) const { return _items[index].name; };
    const bool IsDir(int index) const { return _items[index].is_dir; };
    size_t GetSize();

private:
    std::vector<DirItem> _items;
    std::unordered_set<std::string> _ext_filters;
    static std::unordered_set<std::string> _ext_archives;
    std::string _current_path;

    void *_zip_handle;

    bool _LeagleTest(const char *name, int *entry_index = nullptr);
};