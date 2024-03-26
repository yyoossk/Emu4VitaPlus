#include <imgui_vita2d/imgui_vita.h>
#include "browser.h"
#include "log.h"

Browser::Browser(const char *path, const Emulator *emulator)
    : _emulator(emulator)
{
    LogFunctionName;
    _directory = new Directory(path, _emulator->GetValidExtensions());
}

Browser::~Browser()
{
    LogFunctionName;
    delete _directory;
}

static bool GetDirectoryItem(void *data, int idx, const char **out_text)
{
    *out_text = ((Directory *)data)->GetItem(idx);
    return *out_text != nullptr;
}

SCREEN_STATUS Browser::Show()
{
    LogFunctionNameLimited;

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
            ImGui::Text(_directory->GetCurrentPath().c_str());
            auto size = ImGui::GetWindowContentRegionMax();
            auto min_size = ImGui::GetWindowContentRegionMin();
            ImGui::SetNextItemWidth(size.x * 0.5f);
            LogInfoLimited("%f %f %f %f", size.x, size.y, min_size.x, min_size.y);
            static int item_current = 0;
            ImGui::ListBox("", &item_current, GetDirectoryItem, _directory, _directory->GetSize(), size.y - min_size.y * 4);

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