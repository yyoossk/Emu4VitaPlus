#include "string.h"
#include "core_options.h"
#include "log.h"

#define CORE_SECTION "CORE"

CoreOptions *gCoreOptions = nullptr;

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

CoreOptions::CoreOptions()
{
    LogFunctionName;
}

CoreOptions::~CoreOptions()
{
    LogFunctionName;
    Save();
}

bool CoreOptions::Save(const char *path)
{
    LogFunctionName;
    // toml::table options;

    // for (auto const &option : Options)
    // {
    //     options.insert(option.first, option.second.value);
    // }

    // std::stringstream s;
    // s << toml::table{{CORE_SECTION, options}};

    // FILE *fp = fopen(path, "wb");
    // if (fp)
    // {
    //     fputs(s.str().c_str(), fp);
    //     fclose(fp);
    //     return true;
    // }

    return false;
}

void CoreOptions::Load(retro_core_options_intl *options)
{
    LogFunctionName;
    const retro_core_option_definition *us = options->us;
    const retro_core_option_definition *local = options->local;
    while (us->key)
    {
        const auto &iter = Options.find(us->key);
        CoreOption *option;
        if (iter == Options.end())
        {
            option = &(Options[us->key] = CoreOption{us->desc,
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
    for (auto &iter : Options)
    {
        iter.second.Default();
    }
}