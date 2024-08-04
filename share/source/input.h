#pragma once
#include <stdint.h>
#include <functional>
#include <map>
#include <unordered_map>
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

#define DEFAULT_TURBO_INTERVAL 100000ull
#define DEFAULT_TURBO_START_TIME 500000ull

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

    uint32_t GetKeyStates() { return _last_key; };
    // void ClearKeyStates(uint32_t mask);

    void PushCallbacks();
    void PopCallbacks();

private:
    InputMap _key_up_callbacks;
    InputMap _key_down_callbacks;
    std::unordered_map<uint32_t, uint64_t> _next_turbo_times;
    uint32_t _last_key;
    uint32_t _turbo_key;
    uint64_t _turbo_start_ms;
    uint64_t _turbo_interval_ms;
    uint64_t _next_key_up_called_ms;

    std::stack<InputMap> _callback_stack;

    void _ProcTurbo(uint32_t key);
    void _ProcCallbacks(uint32_t key);
};