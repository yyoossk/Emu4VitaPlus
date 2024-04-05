#pragma once
#include <vector>
#include <unordered_set>
#include <string>

struct DirItem
{
    std::string name;
    bool isDir;
};

class Directory
{
public:
    Directory(const char *path = nullptr,
              const char *ext_filters = NULL,
              char split = '|');
    virtual ~Directory();

    bool SetCurrentPath(const std::string &path);
    const std::string &GetCurrentPath() const { return _current_path; };
    const DirItem &GetItem(int index) const { return _items[index]; };
    const std::string &GetItemName(int index) const { return _items[index].name; };
    const bool IsDir(int index) const { return _items[index].isDir; };
    size_t GetSize();

private:
    std::vector<DirItem> _items;
    std::unordered_set<std::string> _ext_filters;
    std::string _current_path;

    void _SetExtensionFilter(const char *exts, char split);
};