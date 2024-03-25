#pragma once
#include <vector>
#include <string>

class Directory
{
public:
    Directory(const char *path = nullptr);
    virtual ~Directory();

    bool Load(const char *path);

private:
    std::vector<std::string> _dirs;
    std::vector<std::string> _files;
}