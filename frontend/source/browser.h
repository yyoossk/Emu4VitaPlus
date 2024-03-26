#pragma once
#include "global.h"
#include "emulator.h"
#include "input.h"
#include "directory.h"

class Browser
{
public:
    Browser(const char *path, const Emulator *emulator);
    virtual ~Browser();
    SCREEN_STATUS Show();

private:
    Input _input;
    const Emulator *_emulator;
    Directory *_directory;
};
