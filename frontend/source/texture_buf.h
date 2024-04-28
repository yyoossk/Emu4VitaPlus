#pragma once
#include <vita2d.h>
#include "circle_buf.h"
#include "log.h"

#define TEXTURE_BUF_COUNT 10

class TextureBuf : public CircleBuf<vita2d_texture *, TEXTURE_BUF_COUNT>
{
public:
    TextureBuf(SceGxmTextureFormat format, size_t width, size_t height)
        : _width(width), _height(height)
    {
        LogFunctionName;
        for (size_t i = 0; i < TEXTURE_BUF_COUNT; i++)
        {
            _buf[i] = vita2d_create_empty_texture_format(width, height, format);
            LogDebug("TextureBuf %i %08x", i, _buf[i]);
        }
    };

    virtual ~TextureBuf()
    {
        LogFunctionName;

        vita2d_wait_rendering_done();
        for (size_t i = 0; i < TEXTURE_BUF_COUNT; i++)
        {
            vita2d_free_texture(_buf[i]);
        }
    }

    size_t GetWidth() const { return _width; };
    size_t GetHeight() const { return _height; };

private:
    size_t _width, _height;
};