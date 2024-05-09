#include "my_imgui.h"
#include "item_selectable.h"

ItemSelectable::ItemSelectable(LanguageString text, LanguageString info)
    : ItemBase(text, info)
{
}

ItemSelectable::~ItemSelectable()
{
}

void ItemSelectable::Show(bool selected)
{
    const char *text = GetText();
    ImGui::Selectable(text, selected);
    ImGui::NextColumn();

    bool is_popup = ImGui::IsPopupOpen(text);

    if (_actived && !is_popup)
    {
        ImGui::OpenPopup(text);
    }

    if (MyBeginCombo(text, _GetOptionString(_GetIndex()), ImGuiComboFlags_NoArrowButton))
    {
        if (!_actived && is_popup)
        {
            ImGui::CloseCurrentPopup();
        }
        for (size_t i = 0; i < _GetTotalCount(); i++)
        {
            ImGui::Selectable(_GetOptionString(i), i == _GetIndex());
        }
        ImGui::EndCombo();
    }

    ImGui::NextColumn();
}

void ItemSelectable::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&ItemSelectable::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&ItemSelectable::_OnKeyDown, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_UP, std::bind(&ItemSelectable::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_DOWN, std::bind(&ItemSelectable::_OnKeyDown, this, input), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&ItemSelectable::_OnClick, this, input));
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&ItemSelectable::_OnCancel, this, input));
}

void ItemSelectable::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}

void ItemSelectable::_OnKeyUp(Input *input)
{
    size_t index = _GetIndex();
    LOOP_MINUS_ONE(index, _GetTotalCount());
    _SetIndex(index);
}

void ItemSelectable::_OnKeyDown(Input *input)
{
    size_t index = _GetIndex();
    LOOP_PLUS_ONE(index, _GetTotalCount());
    _SetIndex(index);
}

void ItemSelectable::_OnClick(Input *input)
{
    _actived = false;
    input->PopCallbacks();
}

void ItemSelectable::_OnCancel(Input *input)
{
    _actived = false;
    input->PopCallbacks();
    _SetIndex(_old_index);
}

void ItemSelectable::OnActive(Input *input)
{
    _actived = true;
    _old_index = _GetIndex();
    input->PushCallbacks();
    SetInputHooks(input);
}