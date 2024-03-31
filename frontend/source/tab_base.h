#pragma once
#include "input.h"

class TabBase
{
public:
    TabBase();
    virtual ~TabBase();

    virtual void SetInputHooks(Input *input) = 0;
    virtual void UnsetInputHooks(Input *input) = 0;
    virtual void Show() = 0;

private:
};