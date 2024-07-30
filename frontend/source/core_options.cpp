#include "string.h"
#include "SimpleIni.h"
#include "core_options.h"
#include "log.h"

#define CORE_SECTION "CORE"

const std::vector<LanguageString> CoreOption::GetValues() const
{
    std::vector<LanguageString> _values;

    for (const auto &v : values)
    {
        _values.emplace_back(v.label ? v.label : v.value);
    }

    return _values;
}

size_t CoreOption::GetValueIndex()
{
    size_t count = 0;
    for (const auto &v : values)
    {
        if (value == (v.label ? v.label : v.value))
        {
            return count;
        }
        count++;
    }

    return 0;
}

void CoreOption::SetValueIndex(size_t index)
{
    value = values[index].value;
}

void CoreOptions::Load(retro_core_options_intl *options)
{
    LogFunctionName;
    const retro_core_option_definition *us = options->us;
    while (us && us->key)
    {
        _Load(us);
        us++;
    }
}

void CoreOptions::Load(retro_core_options_v2_intl *options)
{
    LogFunctionName;
    if (!options->us)
    {
        return;
    }

    const retro_core_option_v2_definition *us = options->us->definitions;
    while (us && us->key)
    {
        _Load(us);
        us++;
    }
}

void CoreOptions::Load(retro_core_option_definition *options)
{
    LogFunctionName;
    while (options && options->key)
    {
        _Load(options);
        options++;
    }
}

void CoreOptions::Load(retro_core_options_v2 *options)
{
    LogFunctionName;
    const retro_core_option_v2_definition *definitions = options->definitions;
    while (definitions && definitions->key)
    {
        _Load(definitions);
        definitions++;
    }
}

template <typename T>
void CoreOptions::_Load(const T *define)
{
    LogDebug("  key: %s", define->key);
    LogDebug("  desc: %s", define->desc);
    LogDebug("  info: %s", define->info);
    LogDebug("  default_value: %s", define->default_value);
    LogDebug("");

    static const char emptry_string[] = "";

    const auto &iter = this->find(define->key);
    CoreOption *option;
    if (iter == this->end())
    {
        option = &((*this)[define->key] = CoreOption{define->desc ? define->desc : emptry_string,
                                                     define->info ? define->info : emptry_string,
                                                     define->default_value,
                                                     define->default_value});
        // LogDebug("xx %x %s %x %s", define->key, define->key, define->values, define->values->value);
    }
    else
    {
        option = &(iter->second);
        option->desc = define->desc ? define->desc : emptry_string;
        option->info = define->info ? define->info : emptry_string;
        option->values.clear();
    }

    const retro_core_option_value *v = define->values;
    while (v->value)
    {
        option->values.emplace_back(*v);
        v++;
    }
}

void CoreOptions::Default()
{
    LogFunctionName;
    for (auto &iter : *this)
    {
        iter.second.Default();
    }
}

bool CoreOptions::Get(retro_variable *var)
{
    LogFunctionName;
    LogDebug("  key: %s", var->key);
    auto iter = this->find(var->key);
    if (iter == this->end())
    {
        var->value = NULL;
        LogWarn("  %s not found!", var->key);
        return false;
    }
    else
    {
        var->value = iter->second.value.c_str();
        LogDebug("  value: %s", var->value);
        return true;
    }
}

bool CoreOptions::Load(const char *path)
{
    LogFunctionName;
    return true;
}

bool CoreOptions::Save(const char *path)
{
    LogFunctionName;
    CSimpleIniA ini;
    for (auto const &iter : *this)
    {
        const char *key = iter.first.c_str();
        const CoreOption *option = &iter.second;
        ini.SetValue(CORE_SECTION, key, option->value.c_str());
        LogDebug("key %s", key);
        ini.SetValue(key, "desc", option->desc);
        LogDebug("desc %s", option->desc);
        ini.SetValue(key, "info", option->info);
        LogDebug("info %s", option->info);
        ini.SetValue(key, "default_value", option->default_value);
        LogDebug("default_value %s", option->default_value);

        // for (auto vv : option->GetValues())
        // {
        //     LogDebug(" %s", vv.Get());
        // }
        size_t count = 0;
        for (const auto &v : option->values)
        {
            char vs[32];
            snprintf(vs, 32, "value_%d", count);
            ini.SetValue(key, vs, v.value);
            count++;
        }
    }

    return ini.SaveFile(path) == SI_OK;
}

void CoreOptions::SetVisable(const struct retro_core_option_display *option_display)
{
    LogFunctionName;
    auto iter = this->find(option_display->key);
    if (iter != this->end())
    {
        LogDebug("  %s %d", option_display->key, option_display->visible);
        iter->second.visible = option_display->visible;
    }
}