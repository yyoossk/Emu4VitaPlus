#include <psp2/ctrl.h>
#include <string.h>
#include "input.h"
#include "log.h"

#define N_CTRL_PORTS 4
#define ANALOG_CENTER 128
#define ANALOG_THRESHOLD 64

Input::Input() : _last_key(0ull),
                 _turbo_key(0ull),
                 _turbo_start_ms(DEFAULT_TURBO_START_TIME),
                 _turbo_interval_ms(DEFAULT_TURBO_INTERVAL)
{
}

Input::~Input()
{
}

void Input::SetKeyDownCallback(uint64_t key, InputFunc func, bool turbo)
{
    if (func == nullptr)
    {
        UnsetKeyDownCallback(key);
    }
    else
    {
        _key_down_callbacks[key] = func;
        if (turbo)
        {
            _turbo_key |= key;
        }
    }
}

void Input::SetKeyUpCallback(uint64_t key, InputFunc func)
{
    _key_up_callbacks[key] = func;
}

void Input::UnsetKeyUpCallback(uint64_t key)
{
    auto iter = _key_up_callbacks.find(key);
    if (iter != _key_up_callbacks.end())
    {
        _key_up_callbacks.erase(key);
    }
}

void Input::UnsetKeyDownCallback(uint64_t key)
{
    auto iter = _key_down_callbacks.find(key);
    if (iter != _key_down_callbacks.end())
    {
        _key_down_callbacks.erase(key);
    }

    _turbo_key &= ~key;
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

    bool called = false;
    if (key != _last_key)
    {
        for (const auto &iter : _key_down_callbacks)
        {
            if (iter.first & key)
            {
                iter.second();
                called = true;
                _next_turbo_times[iter.first] = sceKernelGetProcessTimeWide() + _turbo_start_ms;
                break;
            }
            else if ((iter.first & _last_key) && (iter.first & _turbo_key))
            {
                auto next = _next_turbo_times.find(iter.first);
                if (next != _next_turbo_times.end())
                {
                    _next_turbo_times.erase(next->first);
                }
            }
        }

        for (const auto &iter : _key_up_callbacks)
        {
            if (iter.first & _last_key)
            {
                iter.second();
                called = true;
                break;
            }
        }
    }

    if (key && !called)
    {
        uint64_t current = sceKernelGetProcessTimeWide();
        for (auto &iter : _next_turbo_times)
        {
            if (current >= iter.second)
            {
                iter.second = current + _turbo_interval_ms;
                _key_down_callbacks[iter.first]();
            }
        }
    }

    _last_key = key;
}

void Input::SetTurboInterval(uint64_t turbo_start, uint64_t turbo_interval)
{
    _turbo_start_ms = turbo_start;
    _turbo_interval_ms = turbo_interval;
};