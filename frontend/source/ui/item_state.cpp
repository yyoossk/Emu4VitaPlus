#include "item_state.h"
#include "defines.h"
#include "log.h"

ItemState::ItemState(State *state)
    : ItemSelectable(""),
      _state(state)
{
  if (strcmp(state->SlotName(), "auto") == 0)
  {
    _text = LanguageString(STATE_AUTO);
  }
  else
  {
    _text = LanguageString(state->SlotName());
  }

  _dialog = new Dialog{"", {DIALOG_OK, DIALOG_CANCEL}, std::bind(&ItemState::_OnRun, this, std::placeholders::_1, std::placeholders::_2)};
}

ItemState::~ItemState()
{
  delete _dialog;
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
  static const TEXT_ENUM MENU_TEXT[] = {STATE_SAVE, STATE_LOAD, STATE_DELETE, STATE_CANCEL};

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

    for (size_t i = 0; i < sizeof(MENU_TEXT) / sizeof(TEXT_ENUM); i++)
    {
      if (i == _index)
      {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
      }
      ImGui::Button(TEXT(MENU_TEXT[i]));
      if (i == _index)
      {
        ImGui::PopStyleColor();
      }
      ImGui::SameLine();
    }

    _dialog->Show();
    ImGui::EndPopup();
  }
}

void ItemState::OnActive(Input *input)
{
  LogFunctionName;
  if (_state->Valid())
  {
    ItemSelectable::OnActive(input);
  }
  else if (strcmp(_state->SlotName(), "auto") != 0)
  {
    _state->Save();
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
  _OnKeyUp(input);
}

void ItemState::_OnKeyRight(Input *input)
{
  _OnKeyDown(input);
}

void ItemState::_OnClick(Input *input)
{
  switch (_index)
  {
  case POPUP_SAVE:
    _dialog->SetText(DIALOG_SAVE_CONFIRM);
    _dialog->OnActive(input);
    break;

  case POPUP_LOAD:
    _dialog->SetText(DIALOG_LOAD_CONFIRM);
    _dialog->OnActive(input);
    break;

  case POPUP_DELETE:
    _dialog->SetText(DIALOG_DELETE_CONFIRM);
    _dialog->OnActive(input);
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

void ItemState::_OnRun(Input *input, int index)
{
  LogFunctionName;

  if (index == 0)
  {
    switch (_index)
    {
    case POPUP_SAVE:
      _state->Save();
      break;
    case POPUP_LOAD:
      _state->Load();
      break;
    case POPUP_DELETE:
      // TODO: delete it
      break;
    }
  }
  _OnCancel(input);
}