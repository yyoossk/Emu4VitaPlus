#include <SimpleIni.h>
#include "input_descriptor.h"
#include "input.h"
#include "log.h"
#include "file.h"
#include "defines.h"

#define INPUT_DESC_SECTION "INPUT_DESC"

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
    if (strcmp(CONSOLE, "ARC") != 0)
    {
        Load();
    }
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
        if (descriptors->port == 0 && descriptors->device == RETRO_DEVICE_JOYPAD && descriptors->id < INPUT_DESC_COUNT)
        {
            _descriptors[descriptors->id].SetDescription(descriptors->description);
        }
        descriptors++;
    }

    if (strcmp(CONSOLE, "ARC") != 0)
    {
        Save();
    }
}

void InputDescriptors::Update()
{
    for (auto &desc : _descriptors)
    {
        desc.Update();
    }
}

const char *InputDescriptors::Get(int index)
{
    if (index < INPUT_DESC_COUNT)
    {
        return _descriptors[index].Get();
    }
    else
    {
        return TEXT(NONE);
    }
}

bool InputDescriptors::Load(const char *path)
{
    LogFunctionName;
    LogDebug("path: %s", path);

    CSimpleIniA ini;
    if (ini.LoadFile(path) != SI_OK)
    {
        return false;
    }

    CSimpleIniA::TNamesDepend keys;
    ini.GetAllKeys(INPUT_DESC_SECTION, keys);
    for (auto const &key : keys)
    {
        int index = std::stoi(key.pItem);
        if (index >= 0 && index < INPUT_DESC_COUNT)
        {
            const char *value = ini.GetValue(INPUT_DESC_SECTION, key.pItem, "NULL");
            _descriptors[index].SetDescription(value);
        }
    }
    return true;
}

bool InputDescriptors::Save(const char *path)
{
    LogFunctionName;
    LogDebug("path: %s", path);

    CSimpleIniA ini;
    char num[8];
    bool need_save = false;
    for (size_t i = 0; i < INPUT_DESC_COUNT; i++)
    {
        const char *s = _descriptors[i].GetDesc();
        if (*s != '\x00')
        {
            need_save = true;
            snprintf(num, 8, "%d", i);
            ini.SetValue(INPUT_DESC_SECTION, num, s);
        }
    }

    if (need_save)
    {
        File::Remove(path);
        return ini.SaveFile(path, false) == SI_OK;
    }
    else
    {
        return false;
    }
}