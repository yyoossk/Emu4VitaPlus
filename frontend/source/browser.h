#pragma once
#include <vector>
#include <string>
#include "global.h"
#include "input.h"

class Browser
{
public:
    SCREEN_STATUS Show();

private:
    Input _input;
};
