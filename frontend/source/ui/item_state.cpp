#include "item_state.h"
#include "defines.h"
#include "log.h"

ItemState::ItemState(State *state)
    : ItemSelectable(""),
      _state(state),
      _dialog_actived(false),
      _dialog_index(0)
{
  if (strcmp(state->SlotName(), "auto") == 0)
  {
    _text = LanguageString(STATE_AUTO);
  }
  else
  {
    _text = LanguageString(state->SlotName());
  }
}

ItemState::~ItemState()
{
}

void ItemState::Show(bool selected)
{
  ImVec2 size = ImGui::GetContentRegionAvail();
  vita2d_texture *texture = _state->Texture();
  float w = vita2d_texture_get_width(texture);
  float h = vita2d_texture_get_height(texture);

  ImGui::Image(texture, {w, h});
  ImGui::SameLine();
  if (selected)
  {
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
  }

  char text[64];
  if (_state->Valid())
  {
    SceDateTime time = _state->CreateTime();
    snprintf(text, 64, "%s (%04d/%02d/%02d %02d:%02d:%02d)", _text.Get(), time.year, time.month, time.day, time.hour, time.minute, time.second);
  }
  else
  {
    snprintf(text, 64, "%s (%s)", _text.Get(), TEXT(STATE_EMPTY));
  }

  ImGui::Button(text, {size.x - w, SCREENSHOT_HEIGHT});

  if (selected)
  {
    ImGui::PopStyleColor();
    _ShowPopup();
  }
}

void ItemState::_ShowPopup()
{
  bool is_popup = ImGui::IsPopupOpen("popup_menu");
  if (_actived && !is_popup)
  {
    ImGui::OpenPopup("popup_menu");
  }

  ImVec2 pos = ImGui::GetCursorScreenPos();
  ImGui::SetNextWindowPos({250.f, pos.y - 50.f});
  if (ImGui::BeginPopupModal("popup_menu", NULL, ImGuiWindowFlags_NoTitleBar))
  {
    if (!_actived && is_popup)
    {
      ImGui::CloseCurrentPopup();
    }
    for (size_t i = 0; i < POPUP_COUNT; i++)
    {
      if (i == _index)
      {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
      }
      ImGui::Button(TEXT(STATE_SAVE + i));
      if (i == _index)
      {
        ImGui::PopStyleColor();
      }
      ImGui::SameLine();
    }

    _ShowDialog();
    ImGui::EndPopup();
  }
}

void ItemState::_ShowDialog()
{
  bool is_popup = ImGui::IsPopupOpen("popup_dialog");
  if (_dialog_actived && !is_popup)
  {
    ImGui::OpenPopup("popup_dialog");
    LogDebug("popup dialog");
  }

  if (ImGui::BeginPopupModal("popup_dialog", NULL, ImGuiWindowFlags_NoTitleBar))
  {

    ImGui::Text("xxx?");
    ImGui::Button("OK");
    ImGui::SameLine();
    ImGui::Button("Cancel");
    ImGui::EndPopup();
  }
}

void ItemState::SetInputHooks(Input *input)
{
  input->SetKeyDownCallback(SCE_CTRL_LEFT, std::bind(&ItemState::_OnKeyLeft, this, input), true);
  input->SetKeyDownCallback(SCE_CTRL_RIGHT, std::bind(&ItemState::_OnKeyRight, this, input), true);
  input->SetKeyDownCallback(SCE_CTRL_LSTICK_LEFT, std::bind(&ItemState::_OnKeyLeft, this, input), true);
  input->SetKeyDownCallback(SCE_CTRL_LSTICK_RIGHT, std::bind(&ItemState::_OnKeyRight, this, input), true);
  input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&ItemState::_OnClick, this, input));
  input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&ItemState::_OnCancel, this, input));
}

void ItemState::UnsetInputHooks(Input *input)
{
  input->UnsetKeyDownCallback(SCE_CTRL_LEFT);
  input->UnsetKeyDownCallback(SCE_CTRL_RIGHT);
  input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_LEFT);
  input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_RIGHT);
  input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
  input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}

void ItemState::_OnKeyLeft(Input *input)
{
  if (_dialog_actived)
  {
    LOOP_MINUS_ONE(_dialog_index, 2);
  }
  else
  {
    _OnKeyUp(input);
  }
}

void ItemState::_OnKeyRight(Input *input)
{
  if (_dialog_actived)
  {
    LOOP_PLUS_ONE(_dialog_index, 2);
  }
  else
  {
    _OnKeyDown(input);
  }
}

void ItemState::_OnClick(Input *input)
{
  LogDebug("%d %d", _index, _dialog_actived);

  switch (_index)
  {
  case POPUP_SAVE:
  case POPUP_LOAD:
  case POPUP_DELETE:
    _dialog_actived = true;
    _dialog_index = 0;
    break;

  case POPUP_CANCEL:
  default:
    _OnCancel(input);
    break;
  }
  LogDebug("  %d %d", _index, _dialog_actived);
}

void ItemState::_OnCancel(Input *input)
{
  if (_dialog_actived)
  {
    _dialog_actived = false;
  }
  else
  {
    _actived = false;
    input->PopCallbacks();
  }
}
