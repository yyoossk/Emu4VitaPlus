#include "emulator.h"
#include "app.h"
#include "video.h"
#include "state_manager.h"
#include "config.h"

void InputPollCallback()
{
    LogFunctionNameLimited;
}

int16_t InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id)
{
    LogFunctionNameLimited;

    // LogDebug("%d %d %d %d", port, device, index, id);

    if (port != 0)
    {
        return 0;
    }

    switch (device)
    {
    case RETRO_DEVICE_JOYPAD:
        return gEmulator->_GetJoypadState(index, id);

    case RETRO_DEVICE_ANALOG:
        return gEmulator->_GetAnalogState(index, id);

    case RETRO_DEVICE_LIGHTGUN:
        return gEmulator->_GetLightGunState(index, id);

    default:
        return 0;
    }
}

int16_t Emulator::_GetJoypadState(unsigned index, unsigned id)
{
    const uint32_t key_states = _input.GetKeyStates();
    if (id == RETRO_DEVICE_ID_JOYPAD_MASK)
    {
        int16_t state = 0;
        int16_t key = 1;
        for (size_t i = 0; i < 16; i++)
        {
            if (key_states & _keys[i])
            {
                state |= key;
            }
            key <<= 1;
        }

        return state;
    }
    else if (id >= 16)
    {
        LogError("  InputStateCallback, wrong id %d", id);
        return 0;
    }
    else
    {
        return (key_states & _keys[id]) ? 1 : 0;
    }
}

int16_t Emulator::_GetAnalogState(unsigned index, unsigned id)
{
    if (_video_rotation == VIDEO_ROTATION_0 || _video_rotation == VIDEO_ROTATION_180)
    {
        const AnalogAxis aa = index == RETRO_DEVICE_INDEX_ANALOG_LEFT ? _input.GetLeftAnalogAxis() : _input.GetRightAnalogAxis();
        return id == RETRO_DEVICE_ID_ANALOG_X ? ANALOG_PSV_TO_RETRO(aa.x) : ANALOG_PSV_TO_RETRO(aa.y);
    }
    else
    {
        const AnalogAxis aa = index == RETRO_DEVICE_INDEX_ANALOG_LEFT ? _input.GetRightAnalogAxis() : _input.GetLeftAnalogAxis();
        return id == RETRO_DEVICE_ID_ANALOG_X ? -(ANALOG_PSV_TO_RETRO(aa.y) + 1) : ANALOG_PSV_TO_RETRO(aa.x);
    }
}

int16_t Emulator::_GetMouseState(unsigned index, unsigned id)
{
    return 0;
}

int16_t Emulator::_GetLightGunState(unsigned index, unsigned id)
{
    if (gEmulator->_input.FrontTouchEnabled())
    {
        switch (id)
        {
        case RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X:
            // LogDebug("%d", _input.GetFrontTouchAxis().x);
            // LogDebug("%08x", (_input.GetFrontTouchAxis().x - _video_rect.x - _video_rect.width / 2) * 0x10000 / _video_rect.width);
            return (_input.GetFrontTouchAxis().x - _video_rect.x - _video_rect.width / 2) * 0x10000 / _video_rect.width;

        case RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y:
            return (_input.GetFrontTouchAxis().y - _video_rect.y - _video_rect.height / 2) * 0x10000 / _video_rect.height;

        default:
            break;
        }
    }

    return 0;
}

void Emulator::SetupKeys()
{
    LogFunctionName;
    memset(_keys, 0, sizeof(_keys));
    _keys_mask = 0;
    _input.Reset();
    for (const auto &k : gConfig->control_maps)
    {
        if (k.retro == 0xff)
        {
            continue;
        }

        if (k.retro >= 16)
        {
            LogError("  wrong key config: %d %08x", k.retro, k.psv);
            continue;
        }

        if (_video_rotation == VIDEO_ROTATION_0 || _video_rotation == VIDEO_ROTATION_180)
        {
            _keys[k.retro] |= k.psv;
        }
        else
        {
            switch (k.retro)
            {
            case RETRO_DEVICE_ID_JOYPAD_UP:
                _keys[RETRO_DEVICE_ID_JOYPAD_RIGHT] |= k.psv;
                break;
            case RETRO_DEVICE_ID_JOYPAD_DOWN:
                _keys[RETRO_DEVICE_ID_JOYPAD_LEFT] |= k.psv;
                break;
            case RETRO_DEVICE_ID_JOYPAD_LEFT:
                _keys[RETRO_DEVICE_ID_JOYPAD_UP] |= k.psv;
                break;
            case RETRO_DEVICE_ID_JOYPAD_RIGHT:
                _keys[RETRO_DEVICE_ID_JOYPAD_DOWN] |= k.psv;
                break;
            default:
                _keys[k.retro] |= k.psv;
                break;
            }
        }

        _keys_mask |= k.psv;
        if (k.turbo)
        {
            _input.SetTurbo(k.psv);
            LogDebug("_input.SetTurbo: %08x", k.psv);
        }
    }

#define BIND_HOTKEY(KEY, FUNC)                                                                   \
    _input.SetKeyDownCallback(gConfig->hotkeys[KEY], std::bind(&Emulator::FUNC, this, &_input)); \
    LogDebug("SetKeyDownCallback " #FUNC " %08x", gConfig->hotkeys[KEY]);

#define BIND_HOTKEY_UP(KEY, FUNC)                                                              \
    _input.SetKeyUpCallback(gConfig->hotkeys[KEY], std::bind(&Emulator::FUNC, this, &_input)); \
    LogDebug("SetKeyUpCallback " #FUNC " %08x", gConfig->hotkeys[KEY]);

    BIND_HOTKEY(SAVE_STATE, _OnHotkeySave);
    BIND_HOTKEY(LOAD_STATE, _OnHotkeyLoad);
    BIND_HOTKEY(GAME_SPEED_UP, _OnHotkeySpeedUp);
    BIND_HOTKEY(GAME_SPEED_DOWN, _OnHotkeySpeedDown);
    BIND_HOTKEY(GAME_REWIND, _OnHotkeyRewind);
    BIND_HOTKEY(CONTROLLER_PORT_UP, _OnHotkeyCtrlPortUp);
    BIND_HOTKEY(CONTROLLER_PORT_DOWN, _OnHotkeyCtrlPortDown);
    BIND_HOTKEY(EXIT_GAME, _OnHotkeyExitGame);

    BIND_HOTKEY_UP(GAME_REWIND, _OnHotkeyRewindUp);
    BIND_HOTKEY_UP(MENU_TOGGLE, _OnPsButton);

    _input.SetTurboInterval(DEFAULT_TURBO_START_TIME, 20000);

    if (gConfig->front_touch)
        _input.EnableFrontTouch(true);

    if (gConfig->rear_touch)
        _input.EnableRearTouch(true);
}

void Emulator::_OnPsButton(Input *input)
{
    LogFunctionName;
    gStatus.Set(APP_STATUS_SHOW_UI_IN_GAME);
    Save();
}

void Emulator::_OnHotkeySave(Input *input)
{
    LogFunctionName;
    Lock();
    gStateManager->states[0]->Save();
    Unlock();
}

void Emulator::_OnHotkeyLoad(Input *input)
{
    LogFunctionName;
    Lock();
    gStateManager->states[0]->Load();
    Unlock();
}

void Emulator::_OnHotkeySpeedUp(Input *input)
{
    LogFunctionName;
    if (_speed < 2.0f)
    {
        SetSpeed(_speed + 0.1);
    }
}

void Emulator::_OnHotkeySpeedDown(Input *input)
{
    LogFunctionName;
    if (_speed > 0.3)
    {
        SetSpeed(_speed - 0.1);
    }
}

void Emulator::_OnHotkeyRewind(Input *input)
{
    if (gConfig->rewind)
    {
        gStatus.Set(APP_STATUS_REWIND_GAME);
    }
}

void Emulator::_OnHotkeyRewindUp(Input *input)
{
    if (gConfig->rewind)
    {
        gStatus.Set(APP_STATUS_RUN_GAME);
    }
}

void Emulator::_OnHotkeyCtrlPortUp(Input *input)
{
    LogFunctionName;
}

void Emulator::_OnHotkeyCtrlPortDown(Input *input)
{
    LogFunctionName;
}

void Emulator::_OnHotkeyExitGame(Input *input)
{
    LogFunctionName;
    UnloadGame();
}

void Emulator::_SetControllerInfo(retro_controller_info *info)
{
    LogFunctionName;

    _controllers.clear();
    unsigned count = 0;
    while (info->types)
    {
        for (unsigned i = 0; i < info->num_types; i++)
        {
            LogDebug(" %d %d %08x %s", count, i, info->types[i].id, info->types[i].desc);
            int device = info->types[i].id & 0xff;
            if (count == 0 && (device == RETRO_DEVICE_JOYPAD ||
                               device == RETRO_DEVICE_MOUSE ||
                               device == RETRO_DEVICE_ANALOG ||
                               device == RETRO_DEVICE_LIGHTGUN ||
                               device == RETRO_DEVICE_POINTER))
            {
                _controllers.push_back(info->types[i].id);
            }
        }
        info++;
        count++;
    }
}