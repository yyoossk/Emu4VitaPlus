#include "emulator.h"
#include "app.h"
#include "video.h"
#include "state_manager.h"
#include "config.h"

void Emulator::_SetupKeys()
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
        _keys[k.retro] |= k.psv;
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
}

void Emulator::_OnPsButton(Input *input)
{
    LogFunctionName;

    sceKernelSignalSema(gEmulator->_video_semaid, 1);
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
        sceKernelSignalSema(gEmulator->_video_semaid, 1);
        gStatus.Set(APP_STATUS_REWIND_GAME);
    }
}

void Emulator::_OnHotkeyRewindUp(Input *input)
{
    if (gConfig->rewind)
    {
        sceKernelSignalSema(gEmulator->_video_semaid, 1);
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