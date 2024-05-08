#ifndef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 0
#endif
#include <toml++/toml.hpp>

#include "core_options.h"
#include "log.h"

#define CORE_SECTION "CORE"

CoreOptions::CoreOptions()
{
    LogFunctionName;
}

CoreOptions::~CoreOptions()
{
    LogFunctionName;
}

bool CoreOptions::Save(const char *path)
{
    LogFunctionName;
    toml::table options;

    for (auto const &option : _options)
    {
        options.insert(option.first, option.second.value);
    }

    std::stringstream s;
    s << toml::table{{CORE_SECTION, options}};

    FILE *fp = fopen(path, "wb");
    if (fp)
    {
        fputs(s.str().c_str(), fp);
        fclose(fp);
        return true;
    }

    return false;
}

void CoreOptions::Load(retro_core_options_intl *options)
{
    LogFunctionName;
    const retro_core_option_definition *us = options->us;
    const retro_core_option_definition *local = options->local;
    while (us->key)
    {
        const auto &iter = _options.find(us->key);
        CoreOption *option;
        if (iter == _options.end())
        {
            option = &(_options[us->key] = CoreOption{us->desc,
                                                      us->info,
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