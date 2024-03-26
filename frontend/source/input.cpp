#include <psp2/ctrl.h>
#include <string.h>
#include "input.h"

#define N_CTRL_PORTS 4
#define ANALOG_CENTER 128
#define ANALOG_THRESHOLD 64

Input::Input() : _last_key(0)
{
}

Input::~Input()
{
}

void Input::SetKeyDownCallback(uint64_t key, InputFunc func)
{
    _callbacks[key] = func;
}

void Input::SetKeyUpCallback(uint64_t key, InputFunc func)
{
    _callbacks[~key] = func;
}

void Input::Poll()
{
    SceCtrlData ctrl_data;

    memset(&ctrl_data, 0, sizeof(SceCtrlData));
    if (sceCtrlPeekBufferPositiveExt2(0, &ctrl_data, 1) < 0)
        return;

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

    if (key != _last_key)
    {
        auto iter = _callbacks.find(key);
        if (iter != _callbacks.end())
        {
            iter->second();
        }
        else
        {
            iter = _callbacks.find(~_last_key);
            if (iter != _callbacks.end())
            {
                iter->second();
            }
        }
    }
    _last_key = key;
}