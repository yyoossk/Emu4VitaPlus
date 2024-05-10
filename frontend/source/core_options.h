#pragma once
#include <map>
#include <vector>
#include <string>
#include "libretro.h"
#include "defines.h"
#include "language_string.h"

struct CoreOption
{
    const char *desc;
    const char *info;
    std::string value;
    const retro_core_option_value *values;
    std::vector<LanguageString> GetValues();
    size_t GetValueIndex();
    void SetValueIndex(size_t index);
};

class CoreOptions
{
public:
    CoreOptions();
    virtual ~CoreOptions();
    bool Load(const char *path = APP_CORE_PATH);
    bool Save(const char *path = APP_CORE_PATH);

    void Load(retro_core_options_intl *options);

    std::map<std::string, CoreOption> Options;
};

extern CoreOptions *gCoreOptions;