#include "core_options.h"
#include "log.h"

CoreOptions::CoreOptions()
{
}

CoreOptions::~CoreOptions()
{
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
        us++;
    }
}