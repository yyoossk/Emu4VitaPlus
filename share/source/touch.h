#pragma once
#include <stdint.h>
#include <vector>
#include <psp2/touch.h>
#include <psp2/kernel/threadmgr.h>
#include "rect.h"

struct TouchAxis
{
    int16_t x;
    int16_t y;
};

class Touch
{
public:
    Touch(SceTouchPortType port);
    virtual ~Touch();

    void Enable(bool enable);
    bool IsEnabled() const { return _enabled; };
    void Poll();
    const bool IsPressed() const { return _last_id != _current_id; };
    int16_t GetCount() const { return _current_id; };
    const TouchAxis &GetAxis() const { return _axis; };
    void InitMovingScale(float xscale, float yscale);

    template <typename T>
    void InitMapTable(const Rect<T> &rect)
    {
        T half_width = rect.width / 2;
        T half_height = rect.height / 2;
        float xscale = 32767.f / half_width; // 0x7fff == 32767
        float yscale = 32767.f / half_height;

        _Lock();

        _map_table_x.clear();
        _map_table_x.reserve(rect.width);
        for (T x = 0; x < rect.width; x++)
        {
            _map_table_x.emplace_back((x - half_width) * xscale);
        }

        _map_table_y.clear();
        _map_table_y.reserve(rect.height);
        for (T y = 0; y < rect.height; y++)
        {
            _map_table_y.emplace_back((y - half_height) * yscale);
        }

        _Unlock();
    }

    template <typename T>
    int16_t GetMapedX(const Rect<T> &rect)
    {
        size_t x = _axis.x - rect.left;
        _Lock();
        int16_t mapx = rect.Contains(_axis.x, _axis.y) && x < _map_table_x.size() ? _map_table_x[x] : -0x8000;
        _Unlock();
        return mapx;
    }

    template <typename T>
    int16_t GetMapedY(const Rect<T> &rect)
    {
        size_t y = _axis.y - rect.top;
        _Lock();
        int16_t mapy = rect.Contains(_axis.x, _axis.y) && y < _map_table_y.size() ? _map_table_y[y] : -0x8000;
        _Unlock();
        return mapy;
    }

    const int16_t GetRelativeMovingX()
    {
        int16_t result = 0;
        if (_last_id == _current_id)
        {
            int x = _axis.x - _last_axis.x;
            _Lock();
            result = x > 0 ? _scale_map_table_x[x] : -_scale_map_table_x[-x];
            _Unlock();
        }
        return result;
    }

    const int16_t GetRelativeMovingY()
    {
        int16_t result = 0;
        if (_last_id == _current_id)
        {
            int y = _axis.y - _last_axis.y;
            _Lock();
            result = y > 0 ? _scale_map_table_y[y] : -_scale_map_table_y[-y];
            _Unlock();
        }
        return result;
    }

private:
    void _Lock() { sceKernelLockLwMutex(&_mutex, 1, NULL); };
    void _Unlock() { sceKernelUnlockLwMutex(&_mutex, 1); };

    bool _enabled;
    static SceTouchPanelInfo _info;
    TouchAxis _last_axis;
    TouchAxis _axis;
    uint8_t _last_id;
    uint8_t _current_id;
    SceTouchPortType _port;
    float _x_scale;
    float _y_scale;

    // map to retro's coordinate system
    // -0x7fff to 0x7fff
    std::vector<int16_t> _map_table_x;
    std::vector<int16_t> _map_table_y;

    // map to retro's mouse moving
    std::vector<float> _scale_map_table_x;
    std::vector<float> _scale_map_table_y;

    SceKernelLwMutexWork _mutex;
};