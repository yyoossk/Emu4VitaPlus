#include "texture_buf.h"
#include "log.h"

TextureBuf::TextureBuf(SceGxmTextureFormat format, size_t width, size_t height, size_t count)
    : _index(0), _width(width), _height(height)
{
    LogFunctionName;
    for (size_t i = 0; i < count; i++)
    {
        _buf.push_back(vita2d_create_empty_texture_format(width, height, format));
        SceKernelLwMutexWork mutex;
        sceKernelCreateLwMutex(&mutex, "texture_buf_mutex", 0, 1, nullptr);
        _mutex.push_back(mutex);
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

void TextureBuf::Next(TextureInfo *info)
{
    _index++;
    _index %= _buf.size();
    Current(info);
}

void TextureBuf::Current(TextureInfo *info)
{
    info->index = _index;
    info->texture = _buf[_index];
}

void TextureBuf::Lock(int index)
{
    sceKernelLockLwMutex(&_mutex[index], 1, nullptr);
}

void TextureBuf::Unlock(int index)
{
    sceKernelUnlockLwMutex(&_mutex[index], 1);
}