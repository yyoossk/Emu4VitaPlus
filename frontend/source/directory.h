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
    const std::string &GetCurrentPath() const { return _currentPath; };
    const DirItem &GetItem(int index) const { return _items[index]; };
    const std::string &GetItemName(int index) const { return _items[index].name; };
    const bool IsDir(int index) const { return _items[index].isDir; };
    int GetSize();

private:
    std::vector<DirItem> _items;
    std::unordered_set<std::string> _extFilters;
    std::string _currentPath;

    void _SetExtensionFilter(const char *exts, char split);
};