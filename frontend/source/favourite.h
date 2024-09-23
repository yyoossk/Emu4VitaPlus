#pragma once
#include <set>
#include <string>
#include "defines.h"

class Favourites : public std::set<std::string>
{
public:
    bool Load(const char *path = APP_FAVOURITE_PATH);
    bool Save(const char *path = APP_FAVOURITE_PATH);

private:
};