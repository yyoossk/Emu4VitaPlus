#include "tab_system.h"
#include "item_config.h"

TabSystem::TabSystem() : TabBase(TAB_SYSTEM)
{
    _items[0] = new ItemConfig<decltype(gConfig->language)>(SYSTEM_MENU_LANGUAGE, &gConfig->language, {LanguageString(gLanguageNames[LANGUAGE_ENGLISH]), LanguageString(gLanguageNames[LANGUAGE_CHINESE])});
    _items[1] = new ItemBase(SYSTEM_MENU_EXIT, std::bind(&TabSystem::_ExitApp, this));
}

TabSystem::~TabSystem()
{
    for (auto &item : _items)
    {
        delete item;
    }
}

void TabSystem::_ActiveItem(Input *input, size_t index)
{
    LogFunctionName;
    _items[index]->OnActive(input);
}

void TabSystem::_ExitApp()
{
    LogFunctionName;
    gStatus = APP_STATUS_EXIT;
}

void TabSystem::_ShowItem(size_t index, bool selected)
{
    _items[index]->Show(selected);
}