#include "input_descriptor.h"
#include "input.h"
#include "log.h"

InputDescriptors gInputDescriptors;

InputDescriptors::InputDescriptors() : _descriptors{BUTTON_B,
                                                    BUTTON_Y,
                                                    BUTTON_SELECT,
                                                    BUTTON_START,
                                                    BUTTON_UP,
                                                    BUTTON_DOWN,
                                                    BUTTON_LEFT,
                                                    BUTTON_RIGHT,
                                                    BUTTON_A,
                                                    BUTTON_X,
                                                    BUTTON_L,
                                                    BUTTON_R,
                                                    BUTTON_L2,
                                                    BUTTON_R2,
                                                    BUTTON_L3,
                                                    BUTTON_R3}
{
}

InputDescriptors::~InputDescriptors()
{
}

void InputDescriptors::UpdateInputDescriptors(const retro_input_descriptor *descriptors)
{
    while (descriptors != nullptr && descriptors->description != nullptr)
    {
        LogDebug("%d %d %d %d %s",
                 descriptors->port,
                 descriptors->device,
                 descriptors->index,
                 descriptors->id,
                 descriptors->description);
        if (descriptors->port == 0 && descriptors->device == RETRO_DEVICE_JOYPAD && descriptors->id < 16)
        {
            _descriptors[descriptors->id].SetDescription(descriptors->description);
        }
        descriptors++;
    }
}

const char *InputDescriptors::Get(int index)
{
    if (index < 16)
    {
        return _descriptors[index].Get();
    }
    else
    {
        return TEXT(NONE);
    }
}