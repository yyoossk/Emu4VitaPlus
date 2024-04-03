#pragma once
#include <stdint.h>
#include <functional>
#include <unordered_map>
#include <libretro.h>

typedef std::function<void()> InputFunc;
const uint64_t SCE_CTRL_LSTICK_UP = 0x0000000100000000;
const uint64_t SCE_CTRL_LSTICK_RIGHT = 0x0000000200000000;
const uint64_t SCE_CTRL_LSTICK_DOWN = 0x0000000400000000;
const uint64_t SCE_CTRL_LSTICK_LEFT = 0x0000000800000000;
const uint64_t SCE_CTRL_RSTICK_UP = 0x0000001000000000;
const uint64_t SCE_CTRL_RSTICK_RIGHT = 0x0000002000000000;
const uint64_t SCE_CTRL_RSTICK_DOWN = 0x0000004000000000;
const uint64_t SCE_CTRL_RSTICK_LEFT = 0x0000008000000000;

#define DEFAULT_TURBO_INTERVAL 100000ull
#define DEFAULT_TURBO_START_TIME 500000ull

#define RETRO_BITMASK_KEY(id) (1 << id)

#define RETRO_JOYPAD_B RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_B)
#define RETRO_JOYPAD_Y RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_Y)
#define RETRO_JOYPAD_SELECT RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_SELECT)
#define RETRO_JOYPAD_START RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_START)
#define RETRO_JOYPAD_UP RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_UP)
#define RETRO_JOYPAD_DOWN RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_DOWN)
#define RETRO_JOYPAD_LEFT RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_LEFT)
#define RETRO_JOYPAD_RIGHT RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_RIGHT)
#define RETRO_JOYPAD_A RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_A)
#define RETRO_JOYPAD_X RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_X)
#define RETRO_JOYPAD_L RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L)
#define RETRO_JOYPAD_R RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R)
#define RETRO_JOYPAD_L2 RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L2)
#define RETRO_JOYPAD_R2 RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R2)
#define RETRO_JOYPAD_L3 RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L3)
#define RETRO_JOYPAD_R3 RETRO_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R3)

class Input
{
public:
    Input();
    virtual ~Input();

    void SetKeyUpCallback(uint64_t key, InputFunc func);
    void SetKeyDownCallback(uint64_t key, InputFunc func, bool turbo = false);

    void UnsetKeyUpCallback(uint64_t key);
    void UnsetKeyDownCallback(uint64_t key);

    void SetTurboInterval(uint64_t turbo_start, uint64_t turbo_interval);

    void Poll();

    uint64_t GetKeyStates() { return _last_key; };

private:
    std::unordered_map<uint64_t, InputFunc> _key_up_callbacks;
    std::unordered_map<uint64_t, InputFunc> _key_down_callbacks;
    std::unordered_map<uint64_t, uint64_t> _next_turbo_times;
    uint64_t _last_key;
    uint64_t _turbo_key;
    uint64_t _turbo_start_ms;
    uint64_t _turbo_interval_ms;
};