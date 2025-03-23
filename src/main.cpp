#include "core/dtime.hpp"
#include "core/event.hpp"
#include "core/input.hpp"
#include "core/logger.hpp"
#include "opengl/opengl_context.hpp"
#include "opengl/shaders.hpp"

#include "platform/platform.hpp"

/* math lib */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static bool is_running;
bool shutdown(u16 code, void *sender, void *listener_inst, event_context data);
bool window_resize(u16 code, void *sender, void *listener_inst, event_context data);
bool mouse_move_callback(u16 code, void *sender, void *listener_inst, event_context data);
bool button_pressed_callback(u16 code, void *sender, void *listener_inst, event_context data);

bool firstMouse = true;
f32 yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
f32 pitch = 0.0f;
f32 lastX = 800.0f / 2.0;
f32 lastY = 600.0 / 2.0;
f32 fov = 45.0f;

// timing
f32 deltaTime = 0.0f; // time between current frame and last frame
f32 lastFrame = 0.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

int main(void)
{
    std::string application_name = "learnOpengl";
    s32 x = 0;
    s32 y = 0;
    s32 win_width = 1270;
    s32 win_height = 800;
    is_running = true;

    /* opengl */
    platform_context plat_state = {};
    opengl_context opengl_context = {};
    clock_context clock_context = {};

    clock_initialize(&clock_context);

    input_initialize();
    event_initialize();
    event_register(EVENT_CODE_APPLICATION_QUIT, NULL, shutdown);
    event_register(EVENT_CODE_RESIZED, NULL, window_resize);
    event_register(EVENT_CODE_MOUSE_MOVED, NULL, mouse_move_callback);
    event_register(EVENT_CODE_KEY_PRESSED, NULL, button_pressed_callback);

    bool result = platform_startup(&plat_state, application_name, x, y, win_width, win_height);
    if (!result)
    {
        FATAL("Failed to startup platform");
        return 1;
    }

    result = init_openGL(&plat_state);
    if (!result)
    {
        ERROR("opengl startup didnt work lol!!");
        return 1;
    }

    opengl_create_shaders(&opengl_context);

    // vertex data and attributes;
    // clang-format off
    f32 vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };
    
    
    //

    // clang-format on

    u32 VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    u32 texture_1;

    glGenTextures(1, &texture_1);
    glBindTexture(GL_TEXTURE_2D, texture_1);
    // set the texture_1 wrapping/filtering options (on the currently bound texture_1 object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture_1

    s32 tex_width, tex_height, tex_nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("../assets/container.jpg", &tex_width, &tex_height, &tex_nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        ERROR("Failed to load texture_1");
    }

    stbi_image_free(data);

    u32 texture_2;

    glGenTextures(1, &texture_2);
    glBindTexture(GL_TEXTURE_2D, texture_2);
    // set the texture_2 wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("../assets/jolly_roger.jpg", &tex_width, &tex_height, &tex_nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        ERROR("Failed to load texture_2");
    }

    stbi_image_free(data);

    glUseProgram(opengl_context.shader_program);
    glUniform1i(glGetUniformLocation(opengl_context.shader_program, "texture_1"), 0);
    glUniform1i(glGetUniformLocation(opengl_context.shader_program, "texture_2"), 1);
    glEnable(GL_DEPTH_TEST);

    // render loop
    // -----------
    while (is_running)
    {
        platform_pump_messages(&plat_state);
        input_update(get_time_milli());

        f32 cameraSpeed = static_cast<float>(2.5 * deltaTime);
        if (input_is_key_down(KEY_W))
        {
            cameraPos += cameraSpeed * cameraFront;
        }
        if (input_is_key_down(KEY_S))
        {
            cameraPos -= cameraSpeed * cameraFront;
        }
        if (input_is_key_down(KEY_A))
        {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (input_is_key_down(KEY_D))
        {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (input_is_key_down(KEY_Z))
        {
            fov -= 1;
            if (fov < 1.0f)
            {
                fov = 1.0f;
            }
        }
        if (input_is_key_down(KEY_X))
        {
            fov += 1;
            if (fov > 45.0f)
            {
                fov = 45.0f;
            }
        }

        f32 currentFrame = static_cast<f32>(get_time_sec());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_2);

        glUseProgram(opengl_context.shader_program);

        glm::mat4 projection = glm::perspective(glm::radians(fov), (f32)win_width / (f32)win_height, 0.1f, 100.0f);
        u32 projection_loc = glGetUniformLocation(opengl_context.shader_program, "projection");

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        u32 view_loc = glGetUniformLocation(opengl_context.shader_program, "view");

        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        for (u32 i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            f32 angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            u32 model_loc = glGetUniformLocation(opengl_context.shader_program, "model");
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        platform_swap_buffers(&plat_state);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(opengl_context.shader_program);

    platform_shutdown(&plat_state);
    event_shutdown();
}

bool shutdown(u16 code, void *sender, void *listener_inst, event_context data)
{
    is_running = false;
    return true;
}
bool window_resize(u16 code, void *sender, void *listener_inst, event_context data)
{
    DEBUG("Window resize msg recieved, new dimensions %d    %d", data.data.u16[0], data.data.u16[1]);
    platform_set_viewport((u32)data.data.u16[0], (u32)data.data.u16[1]);
    return true;
}

bool mouse_move_callback(u16 code, void *sender, void *listener_inst, event_context data)
{
    f32 x_pos = data.data.u16[0];
    f32 y_pos = data.data.u16[1];

    if (firstMouse)
    {
        lastX = x_pos;
        lastY = y_pos;
        firstMouse = false;
    }

    f32 xoffset = x_pos - lastX;
    f32 yoffset = lastY - y_pos; // reversed since y-coordinates go from bottom to top
    lastX = x_pos;
    lastY = y_pos;

    f32 sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    return true;
}
bool button_pressed_callback(u16 code, void *sender, void *listener_inst, event_context data)
{
    keys key = (keys)data.data.u16[0];

    if (code == EVENT_CODE_KEY_PRESSED && key == KEY_ESCAPE)
    {
        return event_fire(EVENT_CODE_APPLICATION_QUIT, nullptr, data);
    }

    DEBUG("Key Pressed %d.", key);

    return true;
}
