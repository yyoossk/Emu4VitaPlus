#include <vita2d.h>
#include <imgui_vita2d/imgui_vita.h>
#include <vector>

#define TEXTURE_BUF_COUNT 4

struct TextureInfo
{
    size_t index;
    vita2d_texture *texture;
};

class TextureBuf
{
public:
    TextureBuf(SceGxmTextureFormat format, size_t width, size_t height, size_t count = TEXTURE_BUF_COUNT);
    virtual ~TextureBuf();
    void Next(TextureInfo *info);
    void Current(TextureInfo *info);
    size_t GetWidth() const { return _width; };
    size_t GetHeight() const { return _height; };
    void Lock(int index);
    void Unlock(int index);

private:
    std::vector<vita2d_texture *> _buf;
    std::vector<SceKernelLwMutexWork> _mutex;
    size_t _index, _width, _height;
};