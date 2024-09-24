#pragma once
#include <set>
#include <string>
#include "directory.h"
#include "defines.h"

struct Favorite
{
    DirItem item;
    std::string path;

    bool operator<(const Favorite &other) const { return item.name < other.item.name; };
};

class Favorites : public std::set<Favorite>
{
public:
    bool Load(const char *path = APP_FAVOURITE_PATH);
    bool Save(const char *path = APP_FAVOURITE_PATH);

private:
};