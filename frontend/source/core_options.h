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
    std::vector<retro_core_option_value> values;

    const std::vector<LanguageString> GetValues() const;
    size_t GetValueIndex();
    void SetValueIndex(size_t index);
    void Default() { value = default_value; };
};

class CoreOptions : public std::map<std::string, CoreOption>
{
public:
    bool Load(const char *path = APP_CORE_CONFIG_PATH);
    bool Save(const char *path = APP_CORE_CONFIG_PATH);

    void Load(retro_core_options_intl *options);
    void Load(retro_core_options_v2_intl *options);
    void Load(retro_core_option_definition *options);
    bool Get(retro_variable *var);
    void Default();

private:
    template <typename T>
    void _Load(const T *define);
};
