#pragma once
#include "opengl/opengl_context.hpp"

struct shader
{
    GLuint program;
};

void shader_create(shader *shader);

void shader_use(shader *shader);
void shader_destroy(shader *shader);
