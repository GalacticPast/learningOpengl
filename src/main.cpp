#include "platform/platform.hpp"
#include <iostream>

int main(void)
{
    platform_state plat_state = {};
    std::string    application_name = "learnOpengl";
    s32            x = 0;
    s32            y = 0;
    s32            width = 1270;
    s32            height = 800;

    platform_startup(&plat_state, application_name, x, y, width, height);
}
