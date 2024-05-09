#include "item_core.h"

ItemCore::ItemCore(CoreOption *option)
    : ItemSelectable(option->desc, option->info),
      _option(option)
{
  _values = _option->GetValues();
}

ItemCore::~ItemCore()
{
}
