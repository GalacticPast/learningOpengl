#include "platform.hpp"
#include <iostream>

#ifdef LINUX
#ifdef PLATFORM_LINUX_WAYLAND

struct internal_context
{
    u32 id;
};

void initialize_window(platform_context *plat_context)
{
    plat_context->internal_context = malloc(sizeof(internal_context));
    internal_context *context = static_cast<internal_context *>(plat_context->internal_context);
}
#endif

#endif
