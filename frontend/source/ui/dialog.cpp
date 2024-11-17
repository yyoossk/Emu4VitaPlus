#include "dialog.h"
#include "defines.h"
#include "utils.h"
#include "video.h"

Dialog::Dialog(LanguageString text,
               std::vector<LanguageString> options,
               DialogCallbackFunc callback)
    : _text(text),
      _options(std::move(options)),
      _callback(callback),
      _index(0),
      _actived(false)
{
}

Dialog::~Dialog()
{
}

void Dialog::Show()
{
    bool is_popup = ImGui::IsPopupOpen("Dialog");
    if (_actived && !is_popup)
    {
        ImGui::OpenPopup("Dialog");
    }

    if (ImGui::BeginPopupModal("Dialog", NULL, ImGuiWindowFlags_NoTitleBar))
    {
        if (!_actived && is_popup)
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::Text(_text.Get());
        size_t i = 0;
        for (const auto &option : _options)
        {
            if (i == _index)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
            }

            ImGui::Button(option.Get());
            ImGui::SameLine();

            if (i == _index)
            {
                ImGui::PopStyleColor();
            }

            i++;
        }

        ImGui::EndPopup();
    }
}

void Dialog::SetText(LanguageString text)
{
    _text = text;
}

void Dialog::OnActive(Input *input)
{
    gVideo->Lock();
    _actived = true;
    input->PushCallbacks();
    SetInputHooks(input);
    gVideo->Unlock();
}

void Dialog::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_LEFT, std::bind(&Dialog::_OnKeyLeft, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_RIGHT, std::bind(&Dialog::_OnKeyRight, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_LEFT, std::bind(&Dialog::_OnKeyLeft, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_RIGHT, std::bind(&Dialog::_OnKeyRight, this, input), true);
    input->SetKeyUpCallback(EnterButton, std::bind(&Dialog::_OnClick, this, input));
    input->SetKeyUpCallback(CancelButton, std::bind(&Dialog::_OnCancel, this, input));
}

void Dialog::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_LEFT);
    input->UnsetKeyDownCallback(SCE_CTRL_RIGHT);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_LEFT);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_RIGHT);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}

void Dialog::_OnKeyLeft(Input *input)
{
    LOOP_MINUS_ONE(_index, _options.size());
}

void Dialog::_OnKeyRight(Input *input)
{
    LOOP_PLUS_ONE(_index, _options.size());
}

void Dialog::_OnClick(Input *input)
{
    gVideo->Lock();
    _actived = false;
    input->PopCallbacks();
    if (_callback)
    {
        _callback(input, _index);
    }
    gVideo->Unlock();
}

void Dialog::_OnCancel(Input *input)
{
    gVideo->Lock();
    _actived = false;
    input->PopCallbacks();
    gVideo->Unlock();
}