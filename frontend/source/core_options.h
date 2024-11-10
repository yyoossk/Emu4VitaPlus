#pragma once
#include <map>
#include <vector>
#include <string>
#include <SimpleIni.h>
#include <libretro.h>
#include "defines.h"
#include "language_string.h"

struct OptionValue
{
    std::string value;
    std::string label;
};

struct CoreOption
{
    std::string value;
    std::string desc;
    std::string info;
    std::string default_value;
    std::vector<OptionValue> values;
    bool visible = true;

    const std::vector<LanguageString> GetValues() const;
    size_t GetValueIndex();
    void SetValueIndex(size_t index);
    void Default() { value = default_value; };
};

class CoreOptions : public std::map<std::string, CoreOption>
{
public:
    bool Load(const char *path = APP_CORE_CONFIG_PATH);
    bool Load(CSimpleIniA &ini);
    bool Save(const char *path = APP_CORE_CONFIG_PATH);
    bool Save(CSimpleIniA &ini);

    void Load(retro_variable *variables);
    void Load(retro_core_options_intl *options);
    void Load(retro_core_options_v2_intl *options);
    void Load(retro_core_option_definition *options);
    void Load(retro_core_options_v2 *options);
    bool Get(retro_variable *var);
    void Default();
    void SetVisable(const struct retro_core_option_display *option_display);

private:
    template <typename T>
    void _Load(const T *define);
};
