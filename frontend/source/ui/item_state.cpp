#include "item_state.h"
#include "defines.h"
#include "app.h"
#include "log.h"

static const TEXT_ENUM MENU_TEXT[] = {STATE_SAVE, STATE_LOAD, STATE_DELETE, STATE_CANCEL};
static const TEXT_ENUM AUTO_MENU_TEXT[] = {STATE_LOAD, STATE_DELETE, STATE_CANCEL};

ItemState::ItemState(State *state)
    : ItemSelectable(""),
      _state(state)
{
  if (strcmp(state->SlotName(), "auto") == 0)
  {
    _text = LanguageString(STATE_AUTO);
    _menu_texts = AUTO_MENU_TEXT;
    _menu_count = sizeof(AUTO_MENU_TEXT) / sizeof(TEXT_ENUM);
  }
  else
  {
    _text = LanguageString(state->SlotName());
    _menu_texts = MENU_TEXT;
    _menu_count = sizeof(MENU_TEXT) / sizeof(TEXT_ENUM);
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
    const SceDateTime &time = _state->CreateTime();
    snprintf(text, 64, "%s (%04d/%02d/%02d %02d:%02d:%02d)", _text.Get(), time.year, time.month, time.day, time.hour, time.minute, time.second);
  }
  else
  {
    snprintf(text, 64, "%s (%s)", _text.Get(), TEXT(STATE_EMPTY));
  }

  ImGui::Button(text, {size.x - w, h});

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

    for (size_t i = 0; i < _menu_count; i++)
    {
      if (i == _index)
      {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
      }
      ImGui::Button(TEXT(_menu_texts[i]));
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
  size_t index = _index;
  if (_menu_count == sizeof(AUTO_MENU_TEXT) / sizeof(TEXT_ENUM))
  {
    index++;
  }

  if (index == POPUP_CANCEL)
  {
    _OnCancel(input);
    return;
  }

  switch (index)
  {
  case POPUP_SAVE:
    _dialog->SetText(DIALOG_SAVE_CONFIRM);
    break;

  case POPUP_LOAD:
    _dialog->SetText(DIALOG_LOAD_CONFIRM);
    break;

  case POPUP_DELETE:
    _dialog->SetText(DIALOG_DELETE_CONFIRM);
    break;

  default:
    LogError("unknown _index: %d", index);
    _OnCancel(input);
    return;
  }

  _dialog->OnActive(input);
}

void ItemState::_OnCancel(Input *input)
{

  _actived = false;
  input->PopCallbacks();
}

void ItemState::_OnRun(Input *input, int index)
{
  LogFunctionName;

  if (index == 0) // press OK
  {
    index = _index;
    if (_menu_count == sizeof(AUTO_MENU_TEXT) / sizeof(TEXT_ENUM))
    {
      index++;
    }

    switch (index)
    {
    case POPUP_SAVE:
      _state->Save();
      break;
    case POPUP_LOAD:
      _state->Load();
      gStatus = APP_STATUS_RUN_GAME;
      break;
    case POPUP_DELETE:
      _state->Remove();
      break;
    default:
      LogError("Unknown index: %d", index);
    }
  }

  _OnCancel(input);
}