#pragma once

struct KeyMapConfig
{
    uint32_t psv;
    uint8_t retro;
    bool turbo = false;
};

enum DisplaySizeConfig
{
    CONFIG_DISPLAY_SIZE_1X = 0,
    CONFIG_DISPLAY_SIZE_2X,
    CONFIG_DISPLAY_SIZE_3X,
    CONFIG_DISPLAY_SIZE_FULL,
};

enum DisplayRatioConfig
{
    CONFIG_DISPLAY_RATIO_DEFAULT = 0,
    CONFIG_DISPLAY_RATIO_BY_GAME_RESOLUTION,
    CONFIG_DISPLAY_RATIO_BY_DEVICE_SCREEN,
    CONFIG_DISPLAY_RATIO_8_7,
    CONFIG_DISPLAY_RATIO_4_3,
    CONFIG_DISPLAY_RATIO_3_2,
    CONFIG_DISPLAY_RATIO_16_9,
};

enum DisplayRotateConfig
{
    CONFIG_DISPLAY_ROTATE_DISABLE = 0,
    CONFIG_DISPLAY_ROTATE_CW_90,
    CONFIG_DISPLAY_ROTATE_CW_180,
    CONFIG_DISPLAY_ROTATE_CW_270,
    CONFIG_DISPLAY_ROTATE_DEFAULT,
};

enum GraphicsShaderConfig
{
    CONFIG_GRAPHICS_SHADER_DEFAULT = 0,
    CONFIG_GRAPHICS_SHADER_LCD3X,
    CONFIG_GRAPHICS_SHADER_SHARP_BILINEAR_SIMPLE,
    CONFIG_GRAPHICS_SHADER_SHARP_BILINEAR,
    CONFIG_GRAPHICS_SHADER_ADVANCED_AA,
};

enum GraphicsOverlayModeConfig
{
    CONFIG_GRAPHICS_OVERLAY_MODE_OVERLAY = 0,
    CONFIG_GRAPHICS_OVERLAY_MODE_BACKGROUND,
};

struct GraphicsConfig
{
    DisplaySizeConfig size;
    DisplayRatioConfig ratio;
    DisplayRotateConfig rotate;
    GraphicsShaderConfig shader;
    GraphicsOverlayModeConfig overlay;
};
