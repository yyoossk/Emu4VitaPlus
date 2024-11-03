#pragma once
#include <vita2d.h>
#include "log.h"
#include "defines.h"
#include "utils.h"

#define TEXTURE_BUF_COUNT 10

class TextureBuf
{
public:
    TextureBuf(SceGxmTextureFormat format, size_t width, size_t height)
        : _width(width), _height(height), _index(0)
    {
        LogFunctionName;

        LogDebug("%d x %d (%d)", width, height, format);

        for (size_t i = 0; i < TEXTURE_BUF_COUNT; i++)
        {
            _buf[i] = vita2d_create_empty_texture_format(width, height, format);
        }

        sceKernelCreateLwMutex(&_mutex, "texture_buf_mutex", 0, 1, nullptr);
    };

    virtual ~TextureBuf()
    {
        LogFunctionName;

        vita2d_wait_rendering_done();
        for (size_t i = 0; i < TEXTURE_BUF_COUNT; i++)
        {
            vita2d_free_texture(_buf[i]);
        }

        sceKernelDeleteLwMutex(&_mutex);
    }

    size_t GetWidth() const { return _width; };
    size_t GetHeight() const { return _height; };

    void Lock() { sceKernelLockLwMutex(&_mutex, 1, nullptr); };
    void Unlock() { sceKernelUnlockLwMutex(&_mutex, 1); };

    vita2d_texture *Next()
    {
        LOOP_PLUS_ONE(_index, TEXTURE_BUF_COUNT);
        return _buf[_index];
    };

    vita2d_texture *NextBegin()
    {
        size_t index = _index;
        LOOP_PLUS_ONE(index, TEXTURE_BUF_COUNT);
        return _buf[index];
    };

    void NextEnd()
    {
        LOOP_PLUS_ONE(_index, TEXTURE_BUF_COUNT);
    };

    vita2d_texture *Current()
    {
        return _buf[_index];
    };

    void SetFilter(SceGxmTextureFilter filter)
    {
        for (auto &buf : _buf)
        {
            vita2d_texture_set_filters(buf, filter, filter);
        }
    }

private:
    SceKernelLwMutexWork _mutex;
    size_t _width, _height;
    size_t _index;
    vita2d_texture *_buf[TEXTURE_BUF_COUNT];
};