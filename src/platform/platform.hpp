#include "defines.hpp"

struct platform_state
{
    void *internal_state;
};

bool platform_startup(platform_state *plat_state, std::string application_name, s32 x, s32 y, s32 width, s32 height);

bool init_openGL(platform_state *plat_state);
