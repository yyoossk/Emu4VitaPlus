#include <psp2/ctrl.h>
#include <string.h>
#include "input.h"
#include "log.h"

#define N_CTRL_PORTS 4
#define ANALOG_CENTER 128
#define ANALOG_THRESHOLD 64

Input::Input() : _last_key(0ull),
                 _turbo_key(0ull),
                 _last_active(0ull)
{
    _delay = new Delay(DEFAULT_TURBO_INTERVAL);
}

Input::~Input()
{
    delete _delay;
}

void Input::SetKeyDownCallback(uint64_t key, InputFunc func)
{
    _key_down_callbacks[key] = func;
}

void Input::SetKeyUpCallback(uint64_t key, InputFunc func)
{
    _key_up_callbacks[key] = func;
}

void Input::UnsetKeyUpCallback(uint64_t key)
{
    _key_up_callbacks.erase(key);
}

void Input::UnsetKeyDownCallback(uint64_t key)
{
    _key_down_callbacks.erase(key);
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
        for (const auto &iter : _key_down_callbacks)
        {
            if (iter.first & key)
            {
                iter.second();
                _last_active &= key;
                break;
            }
        }

        for (const auto &iter : _key_up_callbacks)
        {
            if (iter.first & _last_key && (iter.first & ~_turbo_key || iter.first & ~_last_active))
            {
                iter.second();
                _last_active &= key;
                break;
            }
        }
    }
    _last_key = key;

    if (_turbo_key && _delay->TimeUp())
    {
        for (const auto &iter : _key_up_callbacks)
        {
            if (iter.first & key & _turbo_key & ~_last_active)
            {
                iter.second();
                _last_active &= key;
                break;
            }
        }
        _last_key &= ~_turbo_key;
        _last_active = 0;
    }
}

void Input::SetKeyTurbo(uint64_t key)
{
    _turbo_key |= key;
}

void Input::UnsetKeyTurbo(uint64_t key)
{
    _turbo_key &= ~key;
}

void Input::SetTurboInterval(uint64_t turbo_interval)
{
    _delay->SetInterval(turbo_interval);
};