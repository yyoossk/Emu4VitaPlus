#include <imgui_vita2d/imgui_impl_vita2d.h>
#include <shared.h>
#include "emulator.h"
#include "config.h"
#include "app.h"
#include "overlay.h"
#include "shader.h"

extern float _vita2d_ortho_matrix[4 * 4];

void Emulator::Show()
{
    LogFunctionNameLimited;
    if (_texture_buf == nullptr)
    {
        sceKernelDelayThread(100000);
        return;
    }

    if (gStatus.Get() & (APP_STATUS_RUN_GAME | APP_STATUS_REWIND_GAME))
    {
        // SceUInt time = 15000;
        // sceKernelWaitSema(_video_semaid, 1, &time);
        sceKernelWaitSema(_video_semaid, 1, NULL);
    }
    // size_t count = 0;
    // while (_current_tex == _texture_buf->Current() && count < 10)
    // {
    //     SceUInt timeout = 1000;
    //     sceKernelWaitSema(_video_semaid, 1, &timeout);
    //     count++;
    // }

    // if (count >= 10 && gStatus == APP_STATUS_RUN_GAME)
    // {
    //     LogDebug("skip frame");
    //     return;
    // }
    // sceKernelWaitSema(_video_semaid, 1, NULL);

    if (gConfig->graphics[GRAPHICS_OVERLAY] > 0 && gConfig->graphics[GRAPHICS_OVERLAY_MODE] == CONFIG_GRAPHICS_OVERLAY_MODE_BACKGROUND)
    {
        vita2d_texture *tex = (*gOverlays)[gConfig->graphics[GRAPHICS_OVERLAY] - 1].Get();
        if (tex)
        {
            vita2d_draw_texture(tex, 0.f, 0.f);
        }
    }

    // _texture_buf->Lock();
    // LogDebug("Show _texture_buf->Current() %08x", _texture_buf->Current());
    _current_tex = _texture_buf->Current();

    vita2d_shader *shader = gConfig->graphics[GRAPHICS_SHADER] > 0 ? (*gShaders)[gConfig->graphics[GRAPHICS_SHADER] - 1].Get() : nullptr;
    if (shader)
    {
        vita2d_set_shader(shader);
        float *texture_size = (float *)vita2d_pool_memalign(2 * sizeof(float), sizeof(float));
        texture_size[0] = _texture_buf->GetWidth();
        texture_size[1] = _texture_buf->GetHeight();
        float *output_size = (float *)vita2d_pool_memalign(2 * sizeof(float), sizeof(float));
        output_size[0] = _video_rect.width;
        output_size[1] = _video_rect.height;
        vita2d_set_vertex_uniform(shader, "IN.texture_size", texture_size, 2);
        vita2d_set_vertex_uniform(shader, "IN.video_size", texture_size, 2);
        vita2d_set_vertex_uniform(shader, "IN.output_size", output_size, 2);
        vita2d_set_fragment_uniform(shader, "IN.texture_size", texture_size, 2);
        vita2d_set_fragment_uniform(shader, "IN.video_size", texture_size, 2);
        vita2d_set_fragment_uniform(shader, "IN.output_size", output_size, 2);
        vita2d_set_wvp_uniform(shader, _vita2d_ortho_matrix);
        // vita2d_draw_texture_part_scale_rotate_generic(_current_tex,
        //                                               VITA_WIDTH / 2, VITA_HEIGHT / 2,
        //                                               _video_rect.x, _video_rect.y,
        //                                               _texture_buf->GetWidth(), _texture_buf->GetHeight(),
        //                                               _video_rect.width / _texture_buf->GetWidth(),
        //                                               _video_rect.height / _texture_buf->GetHeight(),
        //                                               0.f);
    }
    else
    {
        sceGxmSetVertexProgram(vita2d_get_context(), _vita2d_textureVertexProgram);
        sceGxmSetFragmentProgram(vita2d_get_context(), _vita2d_textureFragmentProgram);

        void *vertex_wvp_buffer;
        sceGxmReserveVertexDefaultUniformBuffer(vita2d_get_context(), &vertex_wvp_buffer);
        sceGxmSetUniformDataF(vertex_wvp_buffer, _vita2d_textureWvpParam, 0, 16, _vita2d_ortho_matrix);

        // vita2d_draw_texture_part_scale_rotate(_current_tex,
        //                                       VITA_WIDTH / 2, VITA_HEIGHT / 2,
        //                                       _video_rect.x, _video_rect.y,
        //                                       _texture_buf->GetWidth(), _texture_buf->GetHeight(),
        //                                       _video_rect.width / _texture_buf->GetWidth(),
        //                                       _video_rect.height / _texture_buf->GetHeight(),
        //                                       0.f);
    }

    vita2d_texture_vertex *vertices = (vita2d_texture_vertex *)vita2d_pool_memalign(4 * sizeof(vita2d_texture_vertex), // 4 vertices
                                                                                    sizeof(vita2d_texture_vertex));
    memcpy(vertices, _vertices, 4 * sizeof(vita2d_texture_vertex));

    sceGxmSetFragmentTexture(vita2d_get_context(), 0, &_current_tex->gxm_tex);
    sceGxmSetVertexStream(vita2d_get_context(), 0, vertices);
    sceGxmDraw(vita2d_get_context(), SCE_GXM_PRIMITIVE_TRIANGLE_STRIP, SCE_GXM_INDEX_FORMAT_U16, vita2d_get_linear_indices(), 4);

    // LogDebug("%f %f %f %f", _video_rect.x, _video_rect.y, _video_rect.width, _video_rect.height);
    // LogDebug("%f %f", _video_rect.width / _texture_buf->GetWidth(), _video_rect.height / _texture_buf->GetHeight());

    // _texture_buf->Unlock();

    if (gConfig->graphics[GRAPHICS_OVERLAY] > 0 && gConfig->graphics[GRAPHICS_OVERLAY_MODE] == CONFIG_GRAPHICS_OVERLAY_MODE_OVERLAY)
    {
        vita2d_texture *tex = (*gOverlays)[gConfig->graphics[GRAPHICS_OVERLAY] - 1].Get();
        if (tex)
        {
            vita2d_draw_texture(tex, 0.f, 0.f);
        }
    }
}

bool Emulator::GetCurrentSoftwareFramebuffer(retro_framebuffer *fb)
{
    LogFunctionNameLimited;

    return false;
    // TODO: ...

    if (!fb || _texture_buf == nullptr)
    {
        return false;
    }

    _texture_buf->Lock();
    // LogDebug("GetCurrentSoftwareFramebuffer _texture_buf->Current() %08x", _texture_buf->Current());
    _soft_frame_buf_render = true;
    vita2d_texture *texture = _texture_buf->Current();
    // if (texture == _current_tex)
    // {
    //     // LogWarn("same texture: %x %x", texture, _current_tex);
    //     texture = _texture_buf->Next();
    // }

    fb->data = vita2d_texture_get_datap(texture);
    fb->width = vita2d_texture_get_width(texture);
    fb->height = vita2d_texture_get_height(texture);
    fb->pitch = vita2d_texture_get_stride(texture);
    fb->format = _retro_pixel_format;
    fb->access_flags = RETRO_MEMORY_ACCESS_WRITE | RETRO_MEMORY_ACCESS_READ;
    fb->memory_flags = RETRO_MEMORY_TYPE_CACHED;

    return true;
}

void Emulator::_SetPixelFormat(retro_pixel_format format)
{
    LogFunctionName;

    _retro_pixel_format = format;
    SceGxmTextureFormat old_format = _video_pixel_format;
    switch (format)
    {
    case RETRO_PIXEL_FORMAT_0RGB1555:
        _video_pixel_format = SCE_GXM_TEXTURE_FORMAT_X1U5U5U5_1RGB;
        break;
    case RETRO_PIXEL_FORMAT_XRGB8888:
        _video_pixel_format = SCE_GXM_TEXTURE_FORMAT_X8U8U8U8_1RGB;
        break;
    case RETRO_PIXEL_FORMAT_RGB565:
        _video_pixel_format = SCE_GXM_TEXTURE_FORMAT_U5U6U5_RGB;
        break;
    default:
        LogWarn("  unknown pixel format: %d", format);
        break;
    }

    LogDebug("  _video_pixel_format: %d", _video_pixel_format);
    if (_texture_buf != nullptr && old_format != _video_pixel_format)
    {
        delete _texture_buf;
        _texture_buf = nullptr;
    }
}

void Emulator::_SetVideoSize(uint32_t width, uint32_t height)
{
    if (gConfig->graphics[GRAPHICS_OVERLAY] > 0)
    {
        _video_rect.width = (*gOverlays)[gConfig->graphics[GRAPHICS_OVERLAY] - 1].viewport_width;
        _video_rect.height = (*gOverlays)[gConfig->graphics[GRAPHICS_OVERLAY] - 1].viewport_height;
        return;
    }

    float aspect_ratio = .0f;

    switch (gConfig->graphics[DISPLAY_RATIO])
    {
    case CONFIG_DISPLAY_RATIO_BY_DEVICE_SCREEN:
        aspect_ratio = (float)VITA_WIDTH / (float)VITA_HEIGHT;
        break;

    case CONFIG_DISPLAY_RATIO_8_7:
        aspect_ratio = 8.f / 7.f;
        break;

    case CONFIG_DISPLAY_RATIO_4_3:
        aspect_ratio = 4.f / 3.f;
        break;

    case CONFIG_DISPLAY_RATIO_3_2:
        aspect_ratio = 3.f / 2.f;
        break;

    case CONFIG_DISPLAY_RATIO_16_9:
        aspect_ratio = 16.f / 9.f;
        break;

    case CONFIG_DISPLAY_RATIO_BY_GAME_RESOLUTION:
    default:
        break;
    }

    if (aspect_ratio > 0.f && (width / aspect_ratio) < height)
    {
        width = height * aspect_ratio;
    }
    else
    {
        aspect_ratio = (float)width / (float)height;
    }

    switch (gConfig->graphics[DISPLAY_SIZE])
    {
    case CONFIG_DISPLAY_SIZE_2X:
        width *= 2;
        height *= 2;
        break;

    case CONFIG_DISPLAY_SIZE_3X:
        width *= 3;
        height *= 3;
        break;

    case CONFIG_DISPLAY_SIZE_FULL:
        width = VITA_HEIGHT * aspect_ratio;
        height = VITA_HEIGHT;
        break;

    case CONFIG_DISPLAY_SIZE_1X:
    default:
        break;
    }

    _video_rect.width = width;
    _video_rect.height = height;
}

void Emulator::_SetVertices(float tex_x, float tex_y, float tex_w, float tex_h, float x_scale, float y_scale, float rad)
{
    LogFunctionName;

    const float w_half = (tex_w * x_scale) / 2.0f;
    const float h_half = (tex_h * y_scale) / 2.0f;

    const float u0 = tex_x / tex_w;
    const float v0 = tex_y / tex_h;
    const float u1 = (tex_x + tex_w) / tex_w;
    const float v1 = (tex_y + tex_h) / tex_h;

    _vertices[0].x = -w_half;
    _vertices[0].y = -h_half;
    _vertices[0].z = +0.5f;
    _vertices[0].u = u0;
    _vertices[0].v = v0;

    _vertices[1].x = w_half;
    _vertices[1].y = -h_half;
    _vertices[1].z = +0.5f;
    _vertices[1].u = u1;
    _vertices[1].v = v0;

    _vertices[2].x = -w_half;
    _vertices[2].y = h_half;
    _vertices[2].z = +0.5f;
    _vertices[2].u = u0;
    _vertices[2].v = v1;

    _vertices[3].x = w_half;
    _vertices[3].y = h_half;
    _vertices[3].z = +0.5f;
    _vertices[3].u = u1;
    _vertices[3].v = v1;

    const float c = cosf(rad);
    const float s = sinf(rad);
    for (int i = 0; i < 4; ++i)
    { // Rotate and translate
        float _x = _vertices[i].x;
        float _y = _vertices[i].y;
        _vertices[i].x = _x * c - _y * s + VITA_WIDTH / 2;
        _vertices[i].y = _x * s + _y * c + VITA_HEIGHT / 2;
    }
}