#include <stdint.h>
#include <vita2d.h>
#include <imgui_vita2d/imgui_vita_touch.h>
#include "my_imgui.h"
#include "global.h"

extern SceGxmProgram _binary_assets_imgui_v_cg_gxp_start;
extern SceGxmProgram _binary_assets_imgui_f_cg_gxp_start;
static vita2d_texture *gFontTexture = nullptr;

namespace
{
    void matrix_init_orthographic(float *m,
                                  float left,
                                  float right,
                                  float bottom,
                                  float top,
                                  float near,
                                  float far)
    {
        m[0x0] = 2.0f / (right - left);
        m[0x4] = 0.0f;
        m[0x8] = 0.0f;
        m[0xC] = -(right + left) / (right - left);

        m[0x1] = 0.0f;
        m[0x5] = 2.0f / (top - bottom);
        m[0x9] = 0.0f;
        m[0xD] = -(top + bottom) / (top - bottom);

        m[0x2] = 0.0f;
        m[0x6] = 0.0f;
        m[0xA] = -2.0f / (far - near);
        m[0xE] = (far + near) / (far - near);

        m[0x3] = 0.0f;
        m[0x7] = 0.0f;
        m[0xB] = 0.0f;
        m[0xF] = 1.0f;
    }

    SceGxmShaderPatcherId imguiVertexProgramId;
    SceGxmShaderPatcherId imguiFragmentProgramId;

    SceGxmVertexProgram *_vita2d_imguiVertexProgram;
    SceGxmFragmentProgram *_vita2d_imguiFragmentProgram;

    const SceGxmProgramParameter *_vita2d_imguiWvpParam;

    float ortho_proj_matrix[16];

    constexpr auto ImguiVertexSize = 20;
}

static void My_Imgui_Create_Font()
{
    // Build texture atlas
    ImGuiIO &io = ImGui::GetIO();
    // Build and load the texture atlas into a texture
    uint32_t *pixels = NULL;
    int width, height;

    ImFontConfig font_config;
    font_config.OversampleH = 1;
    font_config.OversampleV = 1;
    font_config.PixelSnapH = 1;

    io.Fonts->AddFontFromFileTTF(APP_ASSETS_DIR "/" FONT_PVF_NAME,
                                 20.0f,
                                 &font_config,
                                 io.Fonts->GetGlyphRangesJapanese());
    io.Fonts->GetTexDataAsRGBA32((uint8_t **)&pixels, &width, &height);
    gFontTexture = vita2d_create_empty_texture(width, height);
    const auto stride = vita2d_texture_get_stride(gFontTexture) / 4;
    auto texture_data = (uint32_t *)vita2d_texture_get_datap(gFontTexture);

    for (auto y = 0; y < height; ++y)
        for (auto x = 0; x < width; ++x)
            texture_data[y * stride + x] = pixels[y * width + x];

    io.Fonts->TexID = gFontTexture;

    return;
}

static void My_Imgui_Destroy_Font()
{
    if (gFontTexture)
    {
        vita2d_free_texture(gFontTexture);
        gFontTexture = nullptr;
        ImGui::GetIO().Fonts->TexID = nullptr;
    }
}

IMGUI_API void My_ImGui_ImplVita2D_Init()
{
    uint32_t err;

    My_Imgui_Create_Font();

    // check the shaders
    err = sceGxmProgramCheck(&_binary_assets_imgui_v_cg_gxp_start);
    err = sceGxmProgramCheck(&_binary_assets_imgui_f_cg_gxp_start);

    // register programs with the patcher
    err = sceGxmShaderPatcherRegisterProgram(vita2d_get_shader_patcher(),
                                             &_binary_assets_imgui_v_cg_gxp_start,
                                             &imguiVertexProgramId);

    err = sceGxmShaderPatcherRegisterProgram(vita2d_get_shader_patcher(),
                                             &_binary_assets_imgui_f_cg_gxp_start,
                                             &imguiFragmentProgramId);

    // get attributes by name to create vertex format bindings
    const SceGxmProgramParameter *paramTexturePositionAttribute = sceGxmProgramFindParameterByName(&_binary_assets_imgui_v_cg_gxp_start, "aPosition");

    const SceGxmProgramParameter *paramTextureTexcoordAttribute = sceGxmProgramFindParameterByName(&_binary_assets_imgui_v_cg_gxp_start, "aTexcoord");

    const SceGxmProgramParameter *paramTextureColorAttribute = sceGxmProgramFindParameterByName(&_binary_assets_imgui_v_cg_gxp_start, "aColor");

    // create texture vertex format
    SceGxmVertexAttribute textureVertexAttributes[3];
    SceGxmVertexStream textureVertexStreams[1];
    /* x,y,z: 3 float 32 bits */
    textureVertexAttributes[0].streamIndex = 0;
    textureVertexAttributes[0].offset = 0;
    textureVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
    textureVertexAttributes[0].componentCount = 2; // (x, y)
    textureVertexAttributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(paramTexturePositionAttribute);

    /* u,v: 2 floats 32 bits */
    textureVertexAttributes[1].streamIndex = 0;
    textureVertexAttributes[1].offset = 8; // (x, y) * 4 = 12 bytes
    textureVertexAttributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
    textureVertexAttributes[1].componentCount = 2; // (u, v)
    textureVertexAttributes[1].regIndex = sceGxmProgramParameterGetResourceIndex(paramTextureTexcoordAttribute);

    /* r,g,b,a: 4 int 8 bits */
    textureVertexAttributes[2].streamIndex = 0;
    textureVertexAttributes[2].offset =
        16; // (x, y) * 4 + (u, v) * 4 = 20 bytes
    textureVertexAttributes[2].format = SCE_GXM_ATTRIBUTE_FORMAT_U8N;
    textureVertexAttributes[2].componentCount = 4; // (r, g, b, a)
    textureVertexAttributes[2].regIndex = sceGxmProgramParameterGetResourceIndex(paramTextureColorAttribute);

    // 16 bit (short) indices
    textureVertexStreams[0].stride = ImguiVertexSize;
    textureVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

    // create texture shaders
    err = sceGxmShaderPatcherCreateVertexProgram(vita2d_get_shader_patcher(),
                                                 imguiVertexProgramId,
                                                 textureVertexAttributes,
                                                 3,
                                                 textureVertexStreams,
                                                 1,
                                                 &_vita2d_imguiVertexProgram);

    // Fill SceGxmBlendInfo
    static SceGxmBlendInfo blend_info{};
    blend_info.colorFunc = SCE_GXM_BLEND_FUNC_ADD;
    blend_info.alphaFunc = SCE_GXM_BLEND_FUNC_ADD;
    blend_info.colorSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
    blend_info.colorDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blend_info.alphaSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
    blend_info.alphaDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blend_info.colorMask = SCE_GXM_COLOR_MASK_ALL;

    err = sceGxmShaderPatcherCreateFragmentProgram(vita2d_get_shader_patcher(),
                                                   imguiFragmentProgramId,
                                                   SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
                                                   SCE_GXM_MULTISAMPLE_NONE,
                                                   &blend_info,
                                                   &_binary_assets_imgui_v_cg_gxp_start,
                                                   &_vita2d_imguiFragmentProgram);

    // find vertex uniforms by name and cache parameter information
    _vita2d_imguiWvpParam = sceGxmProgramFindParameterByName(&_binary_assets_imgui_v_cg_gxp_start, "wvp");

    matrix_init_orthographic(ortho_proj_matrix, 0.0f, VITA_WIDTH, VITA_HEIGHT, 0.0f, 0.0f, 1.0f);

    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);

    // Setup back-end capabilities flags
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDrawCursor = true;
    io.ClipboardUserData = NULL;

    ImGui_ImplVita2D_InitTouch();
}

IMGUI_API void My_ImGui_ImplVita2D_Shutdown()
{
    My_Imgui_Destroy_Font();
}

IMGUI_API void My_ImGui_ImplVita2D_RenderDrawData(ImDrawData *draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO &io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    auto _vita2d_context = vita2d_get_context();

    vita2d_enable_clipping();

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const auto cmd_list = draw_data->CmdLists[n];
        const auto vtx_buffer = cmd_list->VtxBuffer.Data;
        const auto vtx_size = cmd_list->VtxBuffer.Size;
        const auto idx_buffer = cmd_list->IdxBuffer.Data;
        const auto idx_size = cmd_list->IdxBuffer.Size;

        const auto vertices = vita2d_pool_memalign(vtx_size * ImguiVertexSize, ImguiVertexSize);
        memcpy(vertices, vtx_buffer, vtx_size * ImguiVertexSize);

        static_assert(sizeof(ImDrawIdx) == 2);
        auto indices = (uint16_t *)vita2d_pool_memalign(idx_size * sizeof(ImDrawIdx), sizeof(void *));
        memcpy(indices, idx_buffer, idx_size * sizeof(ImDrawIdx));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const auto pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                vita2d_set_clip_rectangle((int)pcmd->ClipRect.x, (int)pcmd->ClipRect.y, (int)pcmd->ClipRect.z, (int)pcmd->ClipRect.w);

                sceGxmSetVertexProgram(_vita2d_context, _vita2d_imguiVertexProgram);
                sceGxmSetFragmentProgram(_vita2d_context, _vita2d_imguiFragmentProgram);

                auto err = sceGxmSetVertexStream(_vita2d_context, 0, vertices);

                void *vertex_wvp_buffer;
                sceGxmReserveVertexDefaultUniformBuffer(vita2d_get_context(), &vertex_wvp_buffer);
                sceGxmSetUniformDataF(vertex_wvp_buffer, _vita2d_imguiWvpParam, 0, 16, ortho_proj_matrix);

                const auto texture = (vita2d_texture *)pcmd->TextureId;
                err = sceGxmSetFragmentTexture(_vita2d_context, 0, &texture->gxm_tex);

                err = sceGxmDraw(_vita2d_context,
                                 SCE_GXM_PRIMITIVE_TRIANGLES,
                                 SCE_GXM_INDEX_FORMAT_U16,
                                 indices,
                                 pcmd->ElemCount);
            }

            indices += pcmd->ElemCount;
        }
    }

    vita2d_disable_clipping();
}