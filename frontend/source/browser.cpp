#include <imgui_vita2d/imgui_vita.h>
#include "browser.h"
#include "log.h"

SCREEN_STATUS Browser::Show()
{
    ImGui_ImplVita2D_NewFrame();
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
    ImGui::SetNextWindowPos({MAIN_WINDOW_PADDING, MAIN_WINDOW_PADDING});
    ImGui::SetNextWindowSize({SCREEN_WIDTH - MAIN_WINDOW_PADDING * 2, SCREEN_HEIGHT - MAIN_WINDOW_PADDING * 2});
    ImGui::Begin(APP_NAME_STR,
                 NULL,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    // ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDecoration);
    if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Browser"))
        {
            ImGui::BeginGroup();
            auto size = ImGui::GetWindowContentRegionMax();
            auto min_size = ImGui::GetWindowContentRegionMin();
            ImGui::SetNextItemWidth(size.x * 0.5f);
            LogInfoLimited("%f %f %f %f", size.x, size.y, min_size.x, min_size.y);
            const char *items[] = {"Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon"};
            static int item_current = 1;
            ImGui::ListBox("", &item_current, items, IM_ARRAYSIZE(items), size.y - min_size.y * 4);

            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Favorite"))
        {
            ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplVita2D_RenderDrawData(ImGui::GetDrawData());
    return SCREEN_BROWSER;
}