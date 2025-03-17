#pragma once
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

// memory
void *platform_allocate(u64 size, bool aligned);
void  platform_free(void *block, bool aligned);
void *platform_zero_memory(void *block, u64 size);
void *platform_copy_memory(void *dest, const void *source, u64 size);
void *platform_set_memory(void *dest, s32 value, u64 size);

// chage it later
void platform_swap_buffers(platform_state *plat_state);
