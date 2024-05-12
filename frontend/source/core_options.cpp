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
    const retro_core_option_definition *local = options->local;
    while (us->key)
    {
        const auto &iter = this->find(us->key);
        CoreOption *option;
        if (iter == this->end())
        {
            option = &((*this)[us->key] = CoreOption{us->desc,
                                                     us->info,
                                                     us->default_value,
                                                     us->default_value,
                                                     us->values});
        }
        else
        {
            option = &(iter->second);
            option->desc = us->desc;
            option->info = us->info;
            option->values = us->values;
        }
        if (local)
        {
            const retro_core_option_definition *l = local;
            while (l->key)
            {
                if (strcmp(l->key, us->key) == 0)
                {
                    if (l->desc)
                    {
                        option->desc = l->desc;
                    }

                    if (l->info)
                    {
                        option->info = l->info;
                    }

                    if (l->values)
                    {
                        option->values = l->values;
                    }

                    break;
                }

                l++;
            }
        }

        us++;
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
    if (iter != this->end())
    {
        var->value = iter->second.value.c_str();
        LogDebug("  value: %s", var->value);
    }
}