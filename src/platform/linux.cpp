#include "platform.hpp"
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string.h>

void *platform_allocate(u64 size, bool aligned)
{
    return malloc(size);
}
void platform_free(void *block, bool aligned)
{
    free(block);
}
void *platform_zero_memory(void *block, u64 size)
{
    return memset(block, 0, size);
}
void *platform_copy_memory(void *dest, const void *source, u64 size)
{
    return memcpy(dest, source, size);
}
void *platform_set_memory(void *dest, s32 value, u64 size)
{
    return memset(dest, value, size);
}

void platform_log_message(const char *buffer, log_levels level, u32 max_chars)
{
    // clang-format off
    //  https://stackoverflow.com/questions/5412761/using-colors-with-printf
    //                  FATAL  ERROR   DEBUG  WARN    INFO  TRACE 
    u32 level_color[] = { 41,   31  ,   32  ,   33   ,  34  ,  37 };
    
    // clang-format on 
    
    printf("\033[0;%dm %s",level_color[level],buffer);
    printf("\033[0;37m");

}

void platform_get_shaders(std::string *vertex_shader_source, std::string *fragment_shader_source)
{
    std::string vertex_path = "../src/opengl/shaders/vert.glsl";
    std::string frag_path = "../src/opengl/shaders/frag.glsl";

    std::ifstream vert(vertex_path);
    std::ifstream frag(frag_path);
    
    if(!vert.is_open() && !frag.is_open())
    {
        FATAL("Failed to open vert and frag shaders.");
    }
    
    std::ostringstream vert_stream;
    vert_stream << vert.rdbuf(); 
    *vertex_shader_source = vert_stream.str();

    vert.close();

    std::ostringstream frag_stream;
    frag_stream << frag.rdbuf(); 
    *fragment_shader_source = frag_stream.str();

    frag.close();
}
