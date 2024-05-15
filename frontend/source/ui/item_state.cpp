#include "item_state.h"
#include "log.h"

ItemState::ItemState(State *state)
    : ItemSelectable(""),
      _state(state),
      _dialog_actived(false)
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
  if (ImGui::BeginPopup("popup_menu"))
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
    ImGui::EndPopup();
  }
}

void ItemState::_OnClick(Input *input)
{
  // _actived = false;
  // input->PopCallbacks();
  // _dialog_actived = true;
  switch (_index)
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

void ItemState::_OnCancel(Input *input)
{
  _actived = false;
  input->PopCallbacks();
}
