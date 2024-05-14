#include "tab_state.h"
#include "state_manager.h"
#include "defines.h"
#include "log.h"

enum
{
    POPUP_SAVE = 0,
    POPUP_LOAD,
    POPUP_DELETE,
    POPUP_CANCEL,
    POPUP_COUNT,
};

TabState::TabState()
    : TabSeletable(TAB_STATE),
      _actived(false),
      _popup_index(0)
{
    SetVisable(false);
}

TabState::~TabState()
{
}

void TabState::Show(bool selected)
{
    char text[64];
    ImVec2 size = ImGui::GetContentRegionAvail();

    if (ImGui::BeginTabItem(TEXT(_title_id), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::BeginChild(TEXT(_title_id));
        for (size_t i = 0; i < MAX_STATES; i++)
        {
            vita2d_texture *texture = gStateManager->Texture(i);
            float w = vita2d_texture_get_width(texture);
            float h = vita2d_texture_get_height(texture);
            ImGui::Image(texture, {w, h});

            ImGui::SameLine();
            if (i == _index)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
            }

            _GetButtonText(i, text, 64);
            ImGui::Button(text, {size.x - w, SCREENSHOT_HEIGHT});

            if (i == _index)
            {
                ImGui::PopStyleColor();
                _ShowPopup();
                if (ImGui::GetScrollMaxY() > 0.f)
                {
                    ImGui::SetScrollHereY((float)_index / (float)MAX_STATES);
                }
            }
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

void TabState::_GetButtonText(int index, char *text, size_t length)
{
    if (index == AUTO_STATE_INDEX)
    {
        strncpy(text, TEXT(STATE_AUTO), length);
    }
    else
    {
        snprintf(text, length, "%.2d", index);
    }

    size_t s = strlen(text);
    char *t = text + s;

    if (gStateManager->Valid(index))
    {
        SceDateTime time = gStateManager->CreateTime(index);
        snprintf(t, length - s, " (%04d/%02d/%02d %02d:%02d:%02d)", time.year, time.month, time.day, time.hour, time.minute, time.second);
    }
    else
    {
        snprintf(t, length - s, " (%s)", TEXT(STATE_EMPTY));
    }
}

void TabState::_ShowPopup()
{
    bool is_popup = ImGui::IsPopupOpen("popup_menu");
    if (_actived && !is_popup)
    {
        ImGui::OpenPopup("popup_menu");
    }

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::SetNextWindowPos({250.f, pos.y - 50.f});
    if (ImGui::BeginPopup("popup_menu"))
    {
        if (!_actived && is_popup)
        {
            ImGui::CloseCurrentPopup();
        }
        for (size_t i = 0; i < POPUP_COUNT; i++)
        {
            if (i == _popup_index)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
            }
            ImGui::Button(TEXT(STATE_SAVE + i));
            if (i == _popup_index)
            {
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();
        }
        ImGui::EndPopup();
    }
}

void TabState::_SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_LEFT, std::bind(&TabState::_OnKeyLeft, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_RIGHT, std::bind(&TabState::_OnKeyRight, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_LEFT, std::bind(&TabState::_OnKeyLeft, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_RIGHT, std::bind(&TabState::_OnKeyRight, this, input), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&TabState::_OnClick, this, input));
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&TabState::_OnCancel, this, input));
}

void TabState::_UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}

void TabState::_OnActive(Input *input)
{
    _actived = true;
    input->PushCallbacks();
    input->Reset();
    _SetInputHooks(input);
}

void TabState::_OnKeyLeft(Input *input)
{
    LOOP_MINUS_ONE(_popup_index, 4);
}

void TabState::_OnKeyRight(Input *input)
{
    LOOP_PLUS_ONE(_popup_index, 4);
}

void TabState::_OnClick(Input *input)
{
    switch (_popup_index)
    {
    case POPUP_SAVE:
        break;

    case POPUP_LOAD:
        break;

    case POPUP_DELETE:
        break;

    case POPUP_CANCEL:
    default:
        _OnCancel(input);
        break;
    }
}

void TabState::_OnCancel(Input *input)
{
    _actived = false;
    _UnsetInputHooks(input);
    input->PopCallbacks();
}