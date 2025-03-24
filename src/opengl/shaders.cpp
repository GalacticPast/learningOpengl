#include "shaders.hpp"
#include "core/asserts.hpp"
#include "core/logger.hpp"
#include "defines.hpp"

#include "platform/platform.hpp"

void opengl_create_shaders(opengl_context *opengl_context)
{
    DINFO("Creating vertex and fragment shaders");
    std::string vertex_shader_source;
    std::string fragment_shader_source;

    platform_get_shaders(&vertex_shader_source, &fragment_shader_source);

    DEBUG("Compiling vertex shader");
    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const char *vert_c_str = vertex_shader_source.c_str();

    glShaderSource(vertex_shader, 1, &vert_c_str, NULL);
    glCompileShader(vertex_shader);

    s32 success;
    char info_log[512];

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);

        DERROR("Vertex shader compilation failed. %s", info_log);
        debugBreak();
    }
    DEBUG("Vertex shader compiled succesfully");

    DEBUG("Compiling fragment shader");

    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *frag_c_str = fragment_shader_source.c_str();

    glShaderSource(fragment_shader, 1, &frag_c_str, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);

        DERROR("Fragment shader compilation failed. %s", info_log);
        debugBreak();
    }

    opengl_context->shader_program = glCreateProgram();
    glAttachShader(opengl_context->shader_program, vertex_shader);
    glAttachShader(opengl_context->shader_program, fragment_shader);
    glLinkProgram(opengl_context->shader_program);

    glGetShaderiv(opengl_context->shader_program, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(opengl_context->shader_program, 512, NULL, info_log);

        DERROR("Shader progam linking failed. %s", info_log);
        debugBreak();
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    DINFO("Succsefully created vertex and fragment shaders");
}
