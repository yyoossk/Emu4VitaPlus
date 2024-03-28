#include "texture_buf.h"
#include "log.h"

TextureBuf::TextureBuf(SceGxmTextureFormat format, size_t width, size_t height, size_t size)
    : _size(size), _index(0), _width(width), _height(height)
{
    LogFunctionName;

    _buf = new vita2d_texture *[size];
    _mutex = new SceKernelLwMutexWork[size];

    for (size_t i = 0; i < size; i++)
    {
        _buf[i] = vita2d_create_empty_texture_format(width, height, format);
        sceKernelCreateLwMutex(&_mutex[i], "texture_buf_mutex", 0, 1, nullptr);
    }
}

TextureBuf::~TextureBuf()
{
    LogFunctionName;

    vita2d_wait_rendering_done();

    for (size_t i = 0; i < _size; i++)
    {
        vita2d_free_texture(_buf[i]);
        sceKernelDeleteLwMutex(&_mutex[i]);
    }
}

vita2d_texture *TextureBuf::Next()
{
    _index++;
    _index %= _size;
    return _buf[_index];
}

vita2d_texture *TextureBuf::Current()
{
    return _buf[_index];
}

void TextureBuf::Lock()
{
    sceKernelLockLwMutex(&_mutex[_index], 1, nullptr);
}

void TextureBuf::Unlock()
{
    sceKernelUnlockLwMutex(&_mutex[_index], 1);
}