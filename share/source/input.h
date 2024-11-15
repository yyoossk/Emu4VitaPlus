#pragma once
#include <stdint.h>
#include <functional>
#include <vector>
#include <stack>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include "rect.h"

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

#define TEST_KEY(KEY, KEYS) (((KEY) & (KEYS)) == (KEY))
#define ANALOG_PSV_TO_RETRO(X) (((X) - 0x80) << 8)

class Input;

using InputFunc = std::function<void(Input *)>;

struct KeyBinding
{
    uint32_t key;
    InputFunc func;
};

struct AnalogAxis
{
    uint8_t x;
    uint8_t y;
};

struct TouchAxis
{
    int16_t x;
    int16_t y;
};

enum TouchState
{
    TOUCH_NONE = 0,
    TOUCH_DOWN,
    TOUCH_UP
};

struct TurboKeyState
{
    bool down;
    uint64_t next_change_state_time;
};

class Touch
{
public:
    Touch(SceTouchPortType port)
        : _port(port),
          _enabled(false)
    {
    }

    virtual ~Touch() {};

    void Enable(bool enable);
    bool IsEnabled() const { return _enabled; };
    void Poll();
    const TouchState GetState() const { return _last_id == _current_id ? TOUCH_NONE : TOUCH_DOWN; };
    const TouchAxis &GetAxis() const { return _axis; };

    template <typename T>
    void InitMapTable(const Rect<T> &rect)
    {
        T half_width = rect.width / 2;
        T half_height = rect.height / 2;

        _map_table_x.clear();
        _map_table_y.clear();
        _map_table_x.reserve(rect.width);
        _map_table_y.reserve(rect.height);

        float xscale = 32767.f / half_width; // 0x7fff == 32767
        float yscale = 32767.f / half_height;

        for (T x = 0; x < rect.width; x++)
        {
            _map_table_x.emplace_back((x - half_width) * xscale);
        }

        for (T y = 0; y < rect.height; y++)
        {
            _map_table_y.emplace_back((y - half_height) * yscale);
        }
    }

    template <typename T>
    int16_t GetMapedX(const Rect<T> &rect) const
    {
        size_t x = _axis.x - rect.left;
        return rect.Contains(_axis.x, _axis.y) && x < _map_table_x.size() ? _map_table_x[x] : -0x8000;
    }

    template <typename T>
    int16_t GetMapedY(const Rect<T> &rect) const
    {
        size_t y = _axis.y - rect.top;
        return rect.Contains(_axis.x, _axis.y) && y < _map_table_y.size() ? _map_table_y[y] : -0x8000;
    }

private:
    bool _enabled;
    SceTouchPanelInfo _info;
    TouchAxis _axis;
    uint8_t _last_id;
    uint8_t _current_id;
    SceTouchPortType _port;

    // map to retro's coordinate system
    // -0x7fff to 0x7fff
    std::vector<int16_t> _map_table_x;
    std::vector<int16_t> _map_table_y;
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

    void Poll(bool waiting = false);

    void Reset();

    const uint32_t &GetKeyStates() const { return _last_key; };
    const AnalogAxis &GetLeftAnalogAxis() const { return _left_analog; };
    const AnalogAxis &GetRightAnalogAxis() const { return _right_analog; };
    const int16_t GetMapedLeftAnalogX() const { return _analog_map_table[_left_analog.x]; };
    const int16_t GetMapedLeftAnalogY() const { return _analog_map_table[_left_analog.y]; };
    const int16_t GetMapedRightAnalogX() const { return _analog_map_table[_right_analog.x]; };
    const int16_t GetMapedRightAnalogY() const { return _analog_map_table[_right_analog.y]; };

    void PushCallbacks();
    void PopCallbacks();

    Touch &GetFrontTouch() { return _front_touch; };
    Touch &GetRearTouch() { return _rear_touch; };

private:
    std::vector<KeyBinding> _key_up_callbacks;
    std::vector<KeyBinding> _key_down_callbacks;

    TurboKeyState _turbo_key_states[32];
    uint32_t _last_key;
    uint32_t _turbo_key;
    uint64_t _turbo_start_ms;
    uint64_t _turbo_interval_ms;

    bool _enable_key_up;
    AnalogAxis _left_analog;
    AnalogAxis _right_analog;
    // map to retro's analog
    // -0x7fff to 0x7fff
    int16_t _analog_map_table[0x100];

    Touch _front_touch{SCE_TOUCH_PORT_FRONT};
    Touch _rear_touch{SCE_TOUCH_PORT_BACK};

    std::stack<std::vector<KeyBinding>> _callback_stack;

    uint32_t _ProcTurbo(uint32_t key);
    void _ProcCallbacks(uint32_t key);
};