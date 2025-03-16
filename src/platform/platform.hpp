#include "core/logger.hpp"
#include "defines.hpp"

struct platform_state
{
    void *internal_state;
};

bool platform_startup(platform_state *plat_state, std::string application_name, s32 x, s32 y, s32 width, s32 height);
void platform_shutdown(platform_state *plat_state);

bool platform_pump_messages(platform_state *plat_state);

bool init_openGL(platform_state *plat_state);

void platform_log_message(const char *buffer, log_levels level, u32 max_chars);

// chage it later
void draw(platform_state *plat_state);
