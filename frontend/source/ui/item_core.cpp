#include "item_core.h"

ItemCore::ItemCore(CoreOption *option)
    : ItemBase(option->desc, option->info),
      _option(option)
{
}

ItemCore::~ItemCore()
{
}

void ItemCore::Show(bool selected)
{
}