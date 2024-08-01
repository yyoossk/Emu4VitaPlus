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

void Input::SetKeyDownCallback(uint32_t key, InputFunc func, bool turbo)
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

void Input::SetKeyUpCallback(uint32_t key, InputFunc func)
{
    if (func == nullptr)
    {
        UnsetKeyUpCallback(key);
    }
    else
    {
        _key_up_callbacks[key] = func;
    }
}

void Input::UnsetKeyUpCallback(uint32_t key)
{
    auto iter = _key_up_callbacks.find(key);
    if (iter != _key_up_callbacks.end())
    {
        _key_up_callbacks.erase(key);
    }
}

void Input::UnsetKeyDownCallback(uint32_t key)
{
    auto iter = _key_down_callbacks.find(key);
    if (iter != _key_down_callbacks.end())
    {
        _key_down_callbacks.erase(key);
    }

    _turbo_key &= ~key;
}

bool Input::Poll(bool waiting)
{
    SceCtrlData ctrl_data;

    memset(&ctrl_data, 0, sizeof(SceCtrlData));
    int result = waiting ? sceCtrlReadBufferPositiveExt2(0, &ctrl_data, 1) : sceCtrlPeekBufferPositiveExt2(0, &ctrl_data, 1);
    if (result < 0)
        return false;

    uint32_t key = ctrl_data.buttons;
    if (ctrl_data.lx < (ANALOG_CENTER - ANALOG_THRESHOLD))
        key |= SCE_CTRL_LSTICK_LEFT;
    else if (ctrl_data.lx > (ANALOG_CENTER + ANALOG_THRESHOLD))
        key |= SCE_CTRL_LSTICK_RIGHT;

    if (ctrl_data.ly < (ANALOG_CENTER - ANALOG_THRESHOLD))
        key |= SCE_CTRL_LSTICK_UP;
    else if (ctrl_data.ly > (ANALOG_CENTER + ANALOG_THRESHOLD))
        key |= SCE_CTRL_LSTICK_DOWN;

    if (ctrl_data.rx < (ANALOG_CENTER - ANALOG_THRESHOLD))
        key |= SCE_CTRL_RSTICK_LEFT;
    else if (ctrl_data.rx > (ANALOG_CENTER + ANALOG_THRESHOLD))
        key |= SCE_CTRL_RSTICK_RIGHT;

    if (ctrl_data.ry < (ANALOG_CENTER - ANALOG_THRESHOLD))
        key |= SCE_CTRL_RSTICK_UP;
    else if (ctrl_data.ry > (ANALOG_CENTER + ANALOG_THRESHOLD))
        key |= SCE_CTRL_RSTICK_DOWN;

    _ProcTurbo(key);
    _ProcCallbacks(key);

    bool changed = (_last_key != key);
    _last_key = key;
    return changed;
}

void Input::_ProcTurbo(uint32_t key)
{
    if (!_turbo_key)
    {
        return;
    }

    uint64_t current = sceKernelGetProcessTimeWide();
    uint32_t k = 1;
    do
    {
        if (k & _turbo_key)
        {
            if (k & key)
            {
                if (k & ~_last_key)
                {
                    _next_turbo_times[k] = current + _turbo_start_ms;
                }
                else if (current >= _next_turbo_times[k])
                {
                    _last_key &= ~k;
                    _next_turbo_times[k] = current + _turbo_interval_ms;
                }
            }
            else if (k & _last_key)
            {
                _next_turbo_times.erase(k);
            }
        }

        k <<= 1;
    } while (k);
}

void Input::_ProcCallbacks(uint32_t key)
{
    if (key != _last_key)
    {
        for (const auto &iter : _key_down_callbacks)
        {
            if (((iter.first & key) == iter.first) && !(iter.first & _last_key))
            {
                // LogDebug("  call down: %08x %08x", iter.first, iter.second);
                iter.second(this);
                // if (_key_up_callbacks.find(iter.first) == _key_up_callbacks.end())
                // {
                //     ClearKeyStates(iter.first);
                // }
                break;
            }
        }

        for (const auto &iter : _key_up_callbacks)
        {
            if ((iter.first & _last_key) == iter.first && !(iter.first & key))
            {
                // LogDebug("  call up: %08x %08x", iter.first, iter.second);
                iter.second(this);
                break;
            }
        }
    }
}

void Input::SetTurbo(uint32_t key)
{
    _turbo_key |= key;
}

void Input::UnsetTurbo(uint32_t key)
{
    _turbo_key &= ~key;
}

void Input::SetTurboInterval(uint64_t turbo_start, uint64_t turbo_interval)
{
    _turbo_start_ms = turbo_start;
    _turbo_interval_ms = turbo_interval;
}

void Input::Reset()
{
    _turbo_key = 0;
    _key_down_callbacks.clear();
    _key_up_callbacks.clear();
    _turbo_start_ms = DEFAULT_TURBO_START_TIME;
    _turbo_interval_ms = DEFAULT_TURBO_INTERVAL;
}

void Input::PushCallbacks()
{
    _callback_stack.push(_key_up_callbacks);
    _callback_stack.push(_key_down_callbacks);
    _key_down_callbacks.clear();
    _key_up_callbacks.clear();
}

void Input::PopCallbacks()
{
    _key_down_callbacks.clear();
    _key_up_callbacks.clear();
    _key_down_callbacks = _callback_stack.top();
    _callback_stack.pop();
    _key_up_callbacks = _callback_stack.top();
    _callback_stack.pop();
}

void Input::ClearKeyStates(uint32_t mask)
{
    _last_key &= ~mask;
}