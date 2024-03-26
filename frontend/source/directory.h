#pragma once
#include <vector>
#include <unordered_set>
#include <string>

class Directory
{
public:
    Directory(const char *path = nullptr,
              const char *ext_filters = NULL,
              char split = '|');
    virtual ~Directory();

    bool SetCurrentPath(const char *path);
    const std::string &GetCurrentPath() const { return _currentPath; };
    const char *GetItem(int index);
    int GetSize();

private:
    std::vector<std::string> _items;
    std::unordered_set<std::string> _extFilters;
    std::string _currentPath;

    void _SetExtensionFilter(const char *exts, char split);
};