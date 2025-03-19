#pragma once
#include "core/logger.hpp"
#include "defines.hpp"

void platform_log_message(const char *buffer, log_levels level, u32 max_chars);

// memory
void *platform_allocate(u64 size, bool aligned);
void platform_free(void *block, bool aligned);
void *platform_zero_memory(void *block, u64 size);
void *platform_copy_memory(void *dest, const void *source, u64 size);
void *platform_set_memory(void *dest, s32 value, u64 size);

void platform_load_file(const char *filepath, std::string *str);
void platform_get_shaders(std::string *vertex_shader_source, std::string *fragment_shader_source);
