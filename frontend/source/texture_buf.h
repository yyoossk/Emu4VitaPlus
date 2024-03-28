#include <vita2d.h>
#include <imgui_vita2d/imgui_vita.h>

#define TEXTURE_BUF_COUNT 4

class TextureBuf
{
public:
    TextureBuf(SceGxmTextureFormat format, size_t width, size_t height, size_t size = TEXTURE_BUF_COUNT);
    virtual ~TextureBuf();
    vita2d_texture *Next();
    vita2d_texture *Current();
    size_t GetWidth() const { return _width; };
    size_t GetHeight() const { return _height; };
    void Lock();
    void Unlock();

private:
    size_t _size;
    vita2d_texture **_buf;
    SceKernelLwMutexWork *_mutex;
    size_t _index, _width, _height;
};