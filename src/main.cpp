#include "platform/platform.hpp"
#include <iostream>

int main(void)
{
    platform_context plat_context = {};

    initialize_window(&plat_context);

    check_if_correct(&plat_context);
}
