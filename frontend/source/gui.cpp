#include "gui.h"
#include <vita2d.h>

Gui::Gui()
{
    vita2d_init();
}

Gui::~Gui()
{
    vita2d_fini();
}

void Gui::Run()
{
    vita2d_start_drawing();
    vita2d_clear_screen();
    vita2d_end_drawing();
    vita2d_swap_buffers();
}