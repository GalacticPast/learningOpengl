#include "textures.hpp"
#include "core/logger.hpp"
#include "opengl/shaders.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void texture_create(texture *texture, std::string file_path, GLenum tex_type, GLenum slot, GLenum format, GLenum pixel_type)
{

    texture->type = tex_type;

    s32 tex_width, tex_height, tex_nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(file_path.c_str(), &tex_width, &tex_height, &tex_nrChannels, 0);

    glGenTextures(1, &texture->id);

    // Assigns the texture to a Texture Unit
    glActiveTexture(slot);
    glBindTexture(tex_type, texture->id);

    // set the id wrapping/filtering options (on the currently bound id object)
    glTexParameteri(tex_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(tex_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(tex_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(tex_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the id

    if (data)
    {
        glTexImage2D(tex_type, 0, GL_RGB, tex_width, tex_height, 0, format, pixel_type, data);
        glGenerateMipmap(tex_type);
    }
    else
    {
        DERROR("Failed to load id");
    }

    glBindTexture(tex_type, 0);
    stbi_image_free(data);
}
void texture_create_unit(texture *texture, shader *shader, const char *uniform, GLuint unit)
{
    GLuint tex_unit = glGetUniformLocation(shader->program, uniform);
    // Shader needs to be activated before changing the value of a uniform
    shader_use(shader);
    // Sets the value of the uniform
    glUniform1i(tex_unit, unit);
}

void texture_bind(texture *texture)
{
    glBindTexture(texture->type, texture->id);
}
void texture_unbind(texture *texture)
{
    glBindTexture(texture->type, 0);
}
void texture_delete(texture *texture)
{
    glDeleteTextures(1, &texture->id);
}
