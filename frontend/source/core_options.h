#pragma once
#include <unordered_map>
#include <string>
#include "libretro.h"

struct CoreOption
{
    const char *desc;
    const char *info;
    std::string value;
    const retro_core_option_value *values;
};

class CoreOptions
{
public:
    CoreOptions();
    virtual ~CoreOptions();

    bool Save(const char *path);

    void Load(retro_core_options_intl *options);

private:
    std::unordered_map<std::string, CoreOption> _options;
};