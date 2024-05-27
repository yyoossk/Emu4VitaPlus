#include "string.h"
#include "SimpleIni.h"
#include "core_options.h"
#include "log.h"

std::vector<LanguageString> CoreOption::GetValues()
{
    std::vector<LanguageString> _values;
    const retro_core_option_value *v = values;
    while (v->value)
    {
        _values.emplace_back(v->label ? v->label : v->value);
        v++;
    }
    return _values;
}

size_t CoreOption::GetValueIndex()
{
    const retro_core_option_value *v = values;
    size_t count = 0;
    while (v->value)
    {
        if (value == (v->label ? v->label : v->value))
        {
            return count;
        }
        count++;
        v++;
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

template <typename T>
void CoreOptions::_Load(T *define)
{
    LogDebug("  desc: %s", define->desc);
    LogDebug("  info: %s", define->info);
    LogDebug("  default_value: %s", define->default_value);

    const auto &iter = this->find(define->key);
    CoreOption *option;
    if (iter == this->end())
    {
        option = &((*this)[define->key] = CoreOption{define->desc,
                                                     define->info,
                                                     define->default_value,
                                                     define->default_value,
                                                     define->values});
    }
    else
    {
        option = &(iter->second);
        option->desc = define->desc;
        option->info = define->info;
        option->values = define->values;
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

void CoreOptions::Get(retro_variable *var)
{
    LogFunctionName;
    LogDebug("  key: %s", var->key);
    auto iter = this->find(var->key);
    if (iter == this->end())
    {
        var->value = NULL;
        LogWarn("  %s not found!", var->key);
    }
    else
    {
        var->value = iter->second.value.c_str();
        LogDebug("  value: %s", var->value);
    }
}