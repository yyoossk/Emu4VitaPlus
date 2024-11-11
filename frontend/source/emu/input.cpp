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
    {
        const uint32_t key_states = gEmulator->_input.GetKeyStates();

        if (id == RETRO_DEVICE_ID_JOYPAD_MASK)
        {
            int16_t state = 0;
            int16_t key = 1;
            for (size_t i = 0; i < 16; i++)
            {
                if (key_states & gEmulator->_keys[i])
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
            return (key_states & gEmulator->_keys[id]) ? 1 : 0;
        }
    }
    break;

    case RETRO_DEVICE_ANALOG:
    {
        if (gEmulator->_video_rotation == VIDEO_ROTATION_0 || gEmulator->_video_rotation == VIDEO_ROTATION_180)
        {
            const AnalogAxis aa = index == RETRO_DEVICE_INDEX_ANALOG_LEFT ? gEmulator->_input.GetLeftAnalogAxis() : gEmulator->_input.GetRightAnalogAxis();
            return id == RETRO_DEVICE_ID_ANALOG_X ? ANALOG_PSV_TO_RETRO(aa.x) : ANALOG_PSV_TO_RETRO(aa.y);
        }
        else
        {
            const AnalogAxis aa = index == RETRO_DEVICE_INDEX_ANALOG_LEFT ? gEmulator->_input.GetRightAnalogAxis() : gEmulator->_input.GetLeftAnalogAxis();
            return id == RETRO_DEVICE_ID_ANALOG_X ? -(ANALOG_PSV_TO_RETRO(aa.y) + 1) : ANALOG_PSV_TO_RETRO(aa.x);
        }
    }
    break;

    default:
        return 0;
    }
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