#include "defines.hpp"

struct platform_context
{
    void *internal_context;
};

void initialize_window(platform_context *plat_context);
void check_if_correct(platform_context *plat_context);
