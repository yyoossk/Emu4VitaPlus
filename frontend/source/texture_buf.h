#pragma once
#include <vita2d.h>
#include "buf_base.h"
#include "log.h"

#define TEXTURE_BUF_COUNT 4

class TextureBuf : public BufBase<vita2d_texture *, TEXTURE_BUF_COUNT>
{
public:
    TextureBuf(SceGxmTextureFormat format, size_t width, size_t height)
        : _width(width), _height(height)
    {
        LogFunctionName;
        for (auto &buf : _buf)
        {
            buf = vita2d_create_empty_texture_format(width, height, format);
        }
    };

    virtual ~TextureBuf()
    {
        LogFunctionName;

        vita2d_wait_rendering_done();
        for (auto &buf : _buf)
        {
            vita2d_free_texture(buf);
        }
    }

    size_t GetWidth() const { return _width; };
    size_t GetHeight() const { return _height; };

private:
    size_t _width, _height;
};