#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include "language_define.h"
#include "language_string.h"
#include "input.h"
#include "log.h"

class TabBase
{
public:
    TabBase(TEXT_ENUM title_id, bool visable = true);

    virtual ~TabBase();

    virtual void Show(bool selected);
    virtual void SetInputHooks(Input *input) {};
    void UnsetInputHooks(Input *input) {};

    void SetVisable(bool visable) { _visable = visable; };
    bool Visable() { return _visable; };

protected:
    TEXT_ENUM _title_id;
    bool _visable;
};