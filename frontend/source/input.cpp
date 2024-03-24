#include <psp2/ctrl.h>
#include <string.h>
#include "input.h"

#define N_CTRL_PORTS 4
#define ANALOG_CENTER 128
#define ANALOG_THRESHOLD 64

Input::Input()
{
}

Input::~Input()
{
}

void Input::SetCallback(uint64_t key, InputFunc func)
{
    _callbacks[key] = func;
}

void Input::Run()
{
    SceCtrlData ctrl_data;
    for (int i = 0; i < N_CTRL_PORTS; i++)
    {
        memset(&ctrl_data, 0, sizeof(SceCtrlData));
        if (sceCtrlPeekBufferPositiveExt2(i > 0 ? i + 1 : 0, &ctrl_data, 1) < 0)
            continue;

        uint64_t key = ctrl_data.buttons;
        if (ctrl_data.lx < ANALOG_CENTER - ANALOG_THRESHOLD)
            key |= SCE_CTRL_LSTICK_LEFT;
        else if (ctrl_data.lx > ANALOG_CENTER + ANALOG_THRESHOLD)
            key |= SCE_CTRL_LSTICK_RIGHT;

        if (ctrl_data.ly < ANALOG_CENTER - ANALOG_THRESHOLD)
            key |= SCE_CTRL_LSTICK_UP;
        else if (ctrl_data.ly > ANALOG_CENTER + ANALOG_THRESHOLD)
            key |= SCE_CTRL_LSTICK_DOWN;

        if (ctrl_data.rx < ANALOG_CENTER - ANALOG_THRESHOLD)
            key |= SCE_CTRL_RSTICK_LEFT;
        else if (ctrl_data.rx > ANALOG_CENTER + ANALOG_THRESHOLD)
            key |= SCE_CTRL_RSTICK_RIGHT;

        if (ctrl_data.ry < ANALOG_CENTER - ANALOG_THRESHOLD)
            key |= SCE_CTRL_RSTICK_UP;
        else if (ctrl_data.ry > ANALOG_CENTER + ANALOG_THRESHOLD)
            key |= SCE_CTRL_RSTICK_DOWN;

        auto iter = _callbacks.find(key);
        if (iter != _callbacks.end())
        {
            iter->second();
        }
    }
}