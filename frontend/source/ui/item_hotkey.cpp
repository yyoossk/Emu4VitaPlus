#include "item_hotkey.h"

static uint32_t PsvKeys[] = {
    SCE_CTRL_PSBUTTON,
    SCE_CTRL_UP,
    SCE_CTRL_DOWN,
    SCE_CTRL_LEFT,
    SCE_CTRL_RIGHT,
    SCE_CTRL_CROSS,
    SCE_CTRL_TRIANGLE,
    SCE_CTRL_CIRCLE,
    SCE_CTRL_SQUARE,
    SCE_CTRL_SELECT,
    SCE_CTRL_START,
    SCE_CTRL_L1,
    SCE_CTRL_R1,
    SCE_CTRL_L2,
    SCE_CTRL_R2,
    SCE_CTRL_L3,
    SCE_CTRL_R3,
    SCE_CTRL_LSTICK_UP,
    SCE_CTRL_LSTICK_DOWN,
    SCE_CTRL_LSTICK_LEFT,
    SCE_CTRL_LSTICK_RIGHT,
    SCE_CTRL_RSTICK_UP,
    SCE_CTRL_RSTICK_DOWN,
    SCE_CTRL_RSTICK_LEFT,
    SCE_CTRL_RSTICK_RIGHT,
};

#define PSV_KEYS_SIZE (sizeof(PsvKeys) / sizeof(PsvKeys[0]))

ItemHotkey::ItemHotkey(HotKeyConfig index, uint32_t *hotkey)
    : ItemBase(index + HOTKEY_SAVESTATE),
      _hotkey(hotkey),
      _actived(false),
      _index(0){};

ItemHotkey::~ItemHotkey(){};

void ItemHotkey::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&ItemHotkey::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&ItemHotkey::_OnKeyDown, this, input), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&ItemHotkey::_OnClick, this, input));
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&ItemHotkey::_OnQuit, this, input));
}

void ItemHotkey::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}

void ItemHotkey::Show(bool selected)
{
    const char *text = GetText();
    ImGui::Selectable(text, selected);
    ImGui::NextColumn();

    bool is_popup = ImGui::IsPopupOpen(text);

    if (_actived && !is_popup)
    {
        ImGui::OpenPopup(text);
    }

    if (MyBeginCombo(text, _GetPreviewText(), ImGuiComboFlags_NoArrowButton))
    {
        if (!_actived && is_popup)
        {
            ImGui::CloseCurrentPopup();
        }
        for (size_t i = 0; i < PSV_KEYS_SIZE; i++)
        {
            ImGui::Selectable(TEXT(ControlTextMap[PsvKeys[i]]), PsvKeys[i] & *_hotkey);
            if (i == _index)
            {
                ImGui::SetItemDefaultFocus();
                if (ImGui::GetScrollMaxY() > 0.f)
                {
                    ImGui::SetScrollHereY((float)i / (float)PSV_KEYS_SIZE);
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::NextColumn();
}

void ItemHotkey::OnActive(Input *input)
{
    _actived = true;
    input->PushCallbacks();
    SetInputHooks(input);
}

const char *ItemHotkey::_GetPreviewText()
{
    static uint32_t old_hotkey = 0;
    static std::string s;
    if (old_hotkey == *_hotkey)
    {
        return s.c_str();
    }

    old_hotkey = *_hotkey;
    s.clear();
    for (size_t i = 0; i < PSV_KEYS_SIZE; i++)
    {
        uint32_t key = PsvKeys[i];
        if (key & *_hotkey)
        {
            if (s.size() == 0)
            {
                s = TEXT(ControlTextMap[key]);
            }
            else
            {
                s += std::string(" + ") + TEXT(ControlTextMap[key]);
            }
        }
    }

    return s.c_str();
}

void ItemHotkey::_OnKeyUp(Input *input)
{
    if (_index == 0)
    {
        _index = PSV_KEYS_SIZE - 1;
    }
    else
    {
        _index--;
    }
}
void ItemHotkey::_OnKeyDown(Input *input)
{
    if (_index == PSV_KEYS_SIZE - 1)
    {
        _index = 0;
    }
    else
    {
        _index++;
    }
}
void ItemHotkey::_OnClick(Input *input) {}

void ItemHotkey::_OnQuit(Input *input)
{
    _actived = false;
    input->PopCallbacks();
}