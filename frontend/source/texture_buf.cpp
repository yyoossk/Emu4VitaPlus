#include "texture_buf.h"
#include "log.h"

TextureBuf::TextureBuf(SceGxmTextureFormat format, size_t width, size_t height, size_t count)
    : _index(0), _width(width), _height(height)
{
    LogFunctionName;
    for (size_t i = 0; i < count; i++)
    {
        _buf.push_back(vita2d_create_empty_texture_format(width, height, format));
    }
}

TextureBuf::~TextureBuf()
{
    LogFunctionName;

    vita2d_wait_rendering_done();

    for (auto texture : _buf)
    {
        vita2d_free_texture(texture);
    }
}

vita2d_texture *TextureBuf::Next()
{
    _index++;
    _index %= _buf.size();
    return _buf[_index];
}

vita2d_texture *TextureBuf::Current()
{
    return _buf[_index];
}