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
    const std::string &GetCurrentPath() { return _current_path; };
    const char *GetItem(int index);
    int GetSize();

private:
    std::vector<std::string> _dirs;
    std::vector<std::string> _files;
    std::unordered_set<std::string> _ext_filters;
    std::string _current_path;

    void _SetExtensionFilter(const char *exts, char split);
};