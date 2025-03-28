#pragma once
#include "opengl/opengl_context.hpp"
#include "opengl/shaders.hpp"

struct texture
{
    GLuint id;
    GLenum type;
};
void texture_create(texture *texture, std::string file_path, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
void texture_create_unit(texture *texture, shader *shader, const char *uniform, GLuint unit);
void texture_bind(texture *texture);
void texture_unbind(texture *texture);
void texture_delete(texture *texture);
