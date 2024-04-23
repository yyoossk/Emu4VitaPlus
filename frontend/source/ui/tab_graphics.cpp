#include "tab_graphics.h"
#include "item_config.h"

TabGraphics::TabGraphics() : TabBase(TAB_GRAPHICS)
{
    size_t i = 0;
    _items[i++] = new ItemConfig<decltype(gConfig->graphics_config.size)>(GRAPHICS_MENU_DISPLAY_SIZE,
                                                                          &gConfig->graphics_config.size,
                                                                          DISPLAY_SIZE_1X,
                                                                          4);

    _items[i++] = new ItemConfig<decltype(gConfig->graphics_config.ratio)>(GRAPHICS_MENU_ASPECT_RATIO,
                                                                           &gConfig->graphics_config.ratio,
                                                                           ASPECT_RATIO_BY_GAME_RESOLUTION,
                                                                           6);

#ifdef WANT_DISPLAY_ROTATE
    _items[i++] = new ItemConfig(GRAPHICS_MENU_DISPLAY_ROTATE,
                                 (size_t *)&gConfig->graphics_config.rotate,
                                 sizeof(gConfig->graphics_config.rotate),
                                 DISPLAY_ROTATE_DISABLE,
                                 5);
#endif

    _items[i++] = new ItemConfig<decltype(gConfig->graphics_config.shader)>(GRAPHICS_MENU_GRAPHICS_SHADER,
                                                                            &gConfig->graphics_config.shader,
                                                                            SHADER_DEFAULT,
                                                                            5);

    _items[i++] = new ItemConfig<decltype(gConfig->graphics_config.smooth)>(GRAPHICS_MENU_GRAPHICS_SMOOTH,
                                                                            &gConfig->graphics_config.smooth,
                                                                            NO,
                                                                            2);

    _items[i++] = new ItemConfig<decltype(gConfig->graphics_config.overlay_mode)>(GRAPHICS_MENU_OVERLAY_MODE,
                                                                                  &gConfig->graphics_config.overlay_mode,
                                                                                  OVERLAY_MODE_OVERLAY,
                                                                                  2);
}

TabGraphics::~TabGraphics()
{
    for (auto &item : _items)
    {
        delete item;
    }
}

void TabGraphics::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_GRAPHICS), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        // ImGui::ListBoxHeader("", {size.x / 2, size.y});
        ImGui::Columns(2, NULL, false);
        for (size_t i = 0; i < _items.size(); i++)
        {
            _items[i]->Show(i == _index);
        }
        // ImGui::ListBoxFooter();
        ImGui::Columns(1);
        ImGui::EndTabItem();
    }
}

void TabGraphics::_OnClick(Input *input)
{
    _items[_index]->OnActive(input);
}