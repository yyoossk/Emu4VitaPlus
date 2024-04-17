#include "tab_graphics.h"

TabGraphics::TabGraphics() : _index(0)
{
    _items[0] = new MenuItem(GRAPHICS_MENU_DISPLAY_SIZE,
                             (size_t *)&gConfig->graphics_config.size,
                             sizeof(gConfig->graphics_config.size),
                             DISPLAY_SIZE_1X,
                             4);

    _items[1] = new MenuItem(GRAPHICS_MENU_ASPECT_RATIO,
                             (size_t *)&gConfig->graphics_config.ratio,
                             sizeof(gConfig->graphics_config.ratio),
                             ASPECT_RATIO_BY_GAME_RESOLUTION,
                             6);
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
        auto size = ImGui::GetContentRegionAvail();
        ImGui::ListBoxHeader("", {size.x / 2, size.y});
        for (size_t i = 0; i < _items.size(); i++)
        {
            _items[i]->Show(i == _index);
        }
        ImGui::ListBoxFooter();
        ImGui::EndTabItem();
    }
}

void TabGraphics::SetInputHooks(Input *input)
{
}

void TabGraphics::UnsetInputHooks(Input *input)
{
}
