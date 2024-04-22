#include "tab_system.h"
#include "config_item.h"

TabSystem::TabSystem() : TabBase(TAB_SYSTEM)
{
    //_items[0] = new ConfigItem(SYSTEM_MENU_LANGUAGE, &gConfig->language, sizeof(gConfig->language), )
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