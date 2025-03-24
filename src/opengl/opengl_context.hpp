#pragma once
#include "defines.hpp"

#ifdef LINUX
#include "glad/egl.h"
#include "glad/gles2.h"
#endif

#ifdef PLATFORM_WINDOWS
#include "glad/gl.h"
#include "glad/wgl.h"
#endif

typedef struct opengl_context
{
    u32 shader_program;
} opengl_context;
