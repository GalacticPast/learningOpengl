#pragma once
#include "core/logger.hpp"
#include "defines.hpp"
#include "opengl/opengl_context.hpp"

struct platform_context
{
    void *internal_state;
};

bool platform_startup(platform_context *plat_state, std::string application_name, s32 x, s32 y, s32 width, s32 height);
void platform_shutdown(platform_context *plat_state);
bool platform_pump_messages(platform_context *plat_state);

void platform_log_message(const char *buffer, log_levels level, u32 max_chars);

bool init_openGL(platform_context *plat_state);
void platform_swap_buffers(platform_context *plat_state);
void platform_set_viewport(u16 width, u16 height);

// memory
void *platform_allocate(u64 size, bool aligned);
void platform_free(void *block, bool aligned);
void *platform_zero_memory(void *block, u64 size);
void *platform_copy_memory(void *dest, const void *source, u64 size);
void *platform_set_memory(void *dest, s32 value, u64 size);

void platform_load_file(const char *filepath, std::string *str);
void platform_get_shaders(std::string *vertex_shader_source, std::string *fragment_shader_source);
