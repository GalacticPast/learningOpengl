#include <iostream>

#include "core/event.hpp"
#include "opengl/opengl_context.hpp"
#include "opengl/shaders.hpp"
#include "platform/platform.hpp"

static bool is_running;
bool shutdown(u16 code, void *sender, void *listener_inst, event_context data);

int main(void)
{
    platform_state plat_state = {};
    std::string application_name = "learnOpengl";
    s32 x = 0;
    s32 y = 0;
    s32 width = 1270;
    s32 height = 800;
    is_running = true;

    /* opengl */
    opengl_context opengl_context = {};

    event_initialize();
    event_register(EVENT_CODE_APPLICATION_QUIT, NULL, shutdown);

    bool result = platform_startup(&plat_state, application_name, x, y, width, height);
    if (result == false)
    {
        ERROR("Platform startup It didnt work lol!!");
        return 1;
    }
    result = init_openGL(&plat_state);

    glViewPort(x, y, width, height);

    opengl_create_shaders(&opengl_context);

    // vertex data and attributes;
    // clang-format off
    f32 vertices[] = 
    {
        -0.5f, -0.5f, 0.0f, // left
        0.0f,0.5f,0.0f, //up
        0.5f,-0.5f,0.0f, // right
        -0.5f/2, 0.0f,0.0f,
        0.5f/2, 0.0f, 0.0f,
        0.0f,-0.5f,0.0f
    };

    u32 indices[] = 
    {
        0,3,5,
        5,4,2,
        3,1,4
    };
    // clang-format on

    u32 VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    while (is_running)
    {
        if (platform_pump_messages(&plat_state))
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(opengl_context.shader_program);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

            platform_swap_buffers(&plat_state);
        }
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(opengl_context.shader_program);

    platform_shutdown(&plat_state);
    event_shutdown();
}
bool shutdown(u16 code, void *sender, void *listener_inst, event_context data)
{
    is_running = false;
    return true;
}
