#include "shaders.hpp"
#include "core/asserts.hpp"
#include "core/logger.hpp"
#include "defines.hpp"

const char *vertex_shader_source = "#version 330 core\n"
                                   "layout (location = 0) in vec3 aPos;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                   "}\0";
const char *fragment_shader_source = "#version 330 core\n"
                                     "out vec4 FragColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                     "}\n\0";

void opengl_create_shaders(opengl_context *opengl_context)
{
    INFO("Creating vertex and fragment shaders");

    DEBUG("Compiling vertex shader");
    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    s32 success;
    char info_log[512];

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);

        ERROR("Vertex shader compilation failed. %s", info_log);
        debugBreak();
    }
    DEBUG("V;rtex shader compiled succesfully");

    DEBUG("Compiling fragment shader");
    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);

        ERROR("Fragment shader compilation failed. %s", info_log);
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

        ERROR("Shader progam linking failed. %s", info_log);
        debugBreak();
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    INFO("Succsefully created vertex and fragment shaders");
}
