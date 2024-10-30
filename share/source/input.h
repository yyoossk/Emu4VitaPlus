#pragma once
#include <stdint.h>
#include <functional>
#include <map>
#include <stack>
#include <psp2/ctrl.h>

const uint32_t SCE_CTRL_LSTICK_UP = 0x00400000;
const uint32_t SCE_CTRL_LSTICK_RIGHT = 0x00800000;
const uint32_t SCE_CTRL_LSTICK_DOWN = 0x01000000;
const uint32_t SCE_CTRL_LSTICK_LEFT = 0x02000000;
const uint32_t SCE_CTRL_RSTICK_UP = 0x04000000;
const uint32_t SCE_CTRL_RSTICK_RIGHT = 0x08000000;
const uint32_t SCE_CTRL_RSTICK_DOWN = 0x10000000;
const uint32_t SCE_CTRL_RSTICK_LEFT = 0x20000000;

// from https://shinmera.github.io/promptfont/
#define BUTTON_LEFT "↞"
#define BUTTON_UP "↟"
#define BUTTON_RIGHT "↠"
#define BUTTON_DOWN "↡"
#define BUTTON_A "⇓"
#define BUTTON_B "⇒"
#define BUTTON_X "⇐"
#define BUTTON_Y "⇑"
#define BUTTON_L "↜"
#define BUTTON_R "↝"
#define BUTTON_L1 "↰"
#define BUTTON_R1 "↱"
#define BUTTON_L2 "↲"
#define BUTTON_R2 "↳"
#define BUTTON_L3 "⇋"
#define BUTTON_R3 "⇌"
#define BUTTON_SELECT "⇷"
#define BUTTON_START "⇸"
#define BUTTON_HOME ""
#define BUTTON_LEFT_ANALOG_LEFT "↼"
#define BUTTON_LEFT_ANALOG_UP "↾"
#define BUTTON_LEFT_ANALOG_RIGHT "⇀"
#define BUTTON_LEFT_ANALOG_DOWN "⇂"
#define BUTTON_RIGHT_ANALOG_LEFT "↽"
#define BUTTON_RIGHT_ANALOG_UP "↿"
#define BUTTON_RIGHT_ANALOG_RIGHT "⇁"
#define BUTTON_RIGHT_ANALOG_DOWN "⇃"
#define BUTTON_CROSS "⇣"
#define BUTTON_CIRCLE "⇢"
#define BUTTON_SQUARE "⇠"
#define BUTTON_TRIANGLE "⇡"

#define DEFAULT_TURBO_START_TIME 500000ull
#define DEFAULT_TURBO_INTERVAL 100000ull

#define TEST_KEY(KEY, KEYS) (((KEY) & (KEYS)) != KEY)

class Input;

using InputFunc = std::function<void(Input *)>;

struct CompareKey
{
    bool operator()(uint32_t a, uint32_t b) const
    {
        return a > b;
    }
};

using InputMap = std::map<uint32_t, InputFunc, CompareKey>;

struct AnalogAxis
{
    uint8_t x;
    uint8_t y;
};

struct TurboKeyState
{
    bool down;
    uint64_t next_change_state_time;
};

class Input
{
public:
    Input();
    virtual ~Input();

    void SetKeyUpCallback(uint32_t key, InputFunc func);
    void SetKeyDownCallback(uint32_t key, InputFunc func, bool turbo = false);

    void UnsetKeyUpCallback(uint32_t key);
    void UnsetKeyDownCallback(uint32_t key);

    void SetTurbo(uint32_t key);
    void UnsetTurbo(uint32_t key);

    void SetTurboInterval(uint64_t turbo_start, uint64_t turbo_interval);

    bool Poll(bool waiting = false);

    void Reset();

    const uint32_t &GetKeyStates() const { return _last_key; };
    const AnalogAxis &GetLeftAnalogAxis() const { return _left; };
    const AnalogAxis &GetRightAnalogAxis() const { return _right; };

    void PushCallbacks();
    void PopCallbacks();

private:
    InputMap _key_up_callbacks;
    InputMap _key_down_callbacks;

    TurboKeyState _turbo_key_states[32];
    uint32_t _last_key;
    uint32_t _turbo_key;
    uint64_t _turbo_start_ms;
    uint64_t _turbo_interval_ms;

    bool _enable_key_up;
    AnalogAxis _left;
    AnalogAxis _right;

    std::stack<InputMap> _callback_stack;

    uint32_t _ProcTurbo(uint32_t key);
    void _ProcCallbacks(uint32_t key);
};