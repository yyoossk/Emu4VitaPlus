#pragma once
#include <map>
#include <vector>
#include <string>
#include "libretro.h"
#include "defines.h"
#include "language_string.h"

struct CoreOption
{
    const char *desc = nullptr;
    const char *info = nullptr;
    const char *default_value = nullptr;
    std::string value;
    const retro_core_option_value *values = nullptr;

    std::vector<LanguageString> GetValues();
    size_t GetValueIndex();
    void SetValueIndex(size_t index);
    void Default() { value = default_value; };
};

class CoreOptions
{
public:
    CoreOptions();
    virtual ~CoreOptions();
    bool Load(const char *path = APP_CORE_PATH);
    bool Save(const char *path = APP_CORE_PATH);

    void Load(retro_core_options_intl *options);
    void Default();

    std::map<std::string, CoreOption> Options;
};

extern CoreOptions *gCoreOptions;