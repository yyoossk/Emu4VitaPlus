#include <psp2/ctrl.h>
#include <unordered_map>
#include <stdint.h>
#include "item_control.h"

static std::unordered_map<uint32_t, TEXT_ENUM> ControlTextMap = {
    {SCE_CTRL_CROSS, BUTTON_CROSS},
    {SCE_CTRL_TRIANGLE, BUTTON_TRIANGLE},
    {SCE_CTRL_CIRCLE, BUTTON_CIRCLE},
    {SCE_CTRL_SQUARE, BUTTON_SQUARE},
    {SCE_CTRL_SELECT, BUTTON_SELECT},
    {SCE_CTRL_START, BUTTON_START},
    {SCE_CTRL_UP, BUTTON_UP},
    {SCE_CTRL_DOWN, BUTTON_DOWN},
    {SCE_CTRL_LEFT, BUTTON_LEFT},
    {SCE_CTRL_RIGHT, BUTTON_RIGHT},
    {SCE_CTRL_L1, BUTTON_L1},
    {SCE_CTRL_R1, BUTTON_R1},
    {SCE_CTRL_LSTICK_UP, BUTTON_LEFT_ANALOG_UP},
    {SCE_CTRL_LSTICK_DOWN, BUTTON_LEFT_ANALOG_DOWN},
    {SCE_CTRL_LSTICK_LEFT, BUTTON_LEFT_ANALOG_LEFT},
    {SCE_CTRL_LSTICK_RIGHT, BUTTON_LEFT_ANALOG_RIGHT},
};

ItemControl::ItemControl(ControlMapConfig *control_map)
    : ItemBase(ControlTextMap[control_map->psv]),
      _control_map(control_map)
{
}

ItemControl::~ItemControl()
{
}