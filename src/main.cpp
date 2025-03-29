#include "core/dmath.hpp"
#include "core/dtime.hpp"
#include "core/event.hpp"
#include "core/input.hpp"
#include "core/logger.hpp"

#include "opengl/opengl_context.hpp"
#include "opengl/shaders.hpp"
#include "opengl/textures.hpp"

#include "platform/platform.hpp"

static bool is_running;
bool shutdown(u16 code, void *sender, void *listener_inst, event_context data);
bool window_resize(u16 code, void *sender, void *listener_inst, event_context data);
bool mouse_move_callback(u16 code, void *sender, void *listener_inst, event_context data);
bool button_pressed_callback(u16 code, void *sender, void *listener_inst, event_context data);
void update_game();

bool firstMouse = true;
f32 yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
f32 pitch = 0.0f;
f32 last_x = 800.0f / 2.0;
f32 last_y = 600.0 / 2.0;
f32 fov = 45.0f;

vec3 camera_pos = vec3(0.0f, 0.0f, 3.0f);
vec3 camera_front = vec3(0.0f, 0.0f, -1.0f);
vec3 camera_up = vec3(0.0f, 1.0f, 0.0f);

static clock_context frame_clock = {};

int main(void)
{
    clock_start(&frame_clock);

    std::string application_name = "learnOpengl";
    s32 x = 0;
    s32 y = 0;
    s32 win_width = 1270;
    s32 win_height = 800;
    is_running = true;
    f32 frame_time = 1.0f / 60;

    /* opengl */
    platform_context plat_state = {};
    shader shader = {};

    input_initialize();
    event_initialize();
    event_register(EVENT_CODE_APPLICATION_QUIT, NULL, shutdown);
    event_register(EVENT_CODE_RESIZED, NULL, window_resize);
    event_register(EVENT_CODE_MOUSE_MOVED, NULL, mouse_move_callback);
    event_register(EVENT_CODE_KEY_PRESSED, NULL, button_pressed_callback);

    bool result = platform_startup(&plat_state, application_name, x, y, win_width, win_height);
    if (!result)
    {
        DFATAL("Failed to startup platform");
        return 1;
    }

    result = init_openGL(&plat_state);
    if (!result)
    {
        DERROR("opengl startup didnt work lol!!");
        return 1;
    }

    shader_create(&shader);

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

    
    
    
    //

    // clang-format on

    u32 VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    texture box_texture;
    texture_create(&box_texture, "../assets/textures/container.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
    texture_create_unit(&box_texture, &shader, "texture_0", 0);

    glEnable(GL_DEPTH_TEST);

    // render loop
    // -----------
    while (is_running)
    {
        clock_update(&frame_clock);
        platform_pump_messages(&plat_state);
        input_update(0 /* for now sending 0 time*/);
        update_game();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_use(&shader);
        texture_bind(&box_texture);

        mat4 projection = mat4_perspective(deg_to_rad(fov), (f32)win_width / (f32)win_height, 0.1f, 100.0f);
        u32 projection_loc = glGetUniformLocation(shader.program, "projection");

        mat4 view = mat4_look_at(camera_pos, camera_pos + camera_front, camera_up);
        u32 view_loc = glGetUniformLocation(shader.program, "view");

        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.data);
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection.data);

        glBindVertexArray(VAO);

        mat4 model = mat4();
        u32 model_loc = glGetUniformLocation(shader.program, "model");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, model.data);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        platform_swap_buffers(&plat_state);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    texture_delete(&box_texture);
    shader_destroy(&shader);
    clock_stop(&frame_clock);
    platform_shutdown(&plat_state);
    event_shutdown();
}

void update_game()
{
    f32 elapsed = static_cast<f32>(frame_clock.elapsed);
    DINFO("Elapsed since start: %fs", elapsed);
    f32 camera_speed = static_cast<float>(2.5 * elapsed);

    if (input_is_key_down(KEY_W))
    {
        camera_pos += camera_front * camera_speed;
    }
    if (input_is_key_down(KEY_S))
    {
        camera_pos -= camera_front * camera_speed;
    }
    if (input_is_key_down(KEY_A))
    {
        camera_pos -= vec3_normalized(vec3_cross(camera_front, camera_up)) * camera_speed;
    }
    if (input_is_key_down(KEY_D))
    {
        camera_pos += vec3_normalized(vec3_cross(camera_front, camera_up)) * camera_speed;
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
}

bool shutdown(u16 code, void *sender, void *listener_inst, event_context data)
{
    is_running = false;
    return true;
}
bool window_resize(u16 code, void *sender, void *listener_inst, event_context data)
{
    DEBUG("Window resize msg recieved, new dimensions %d    %d", data.data.u32[0], data.data.u32[1]);
    platform_set_viewport((u32)data.data.u32[0], (u32)data.data.u32[1]);
    return true;
}

bool mouse_move_callback(u16 code, void *sender, void *listener_inst, event_context data)
{
    f32 x_pos = data.data.u16[0];
    f32 y_pos = data.data.u16[1];

    if (firstMouse)
    {
        last_x = x_pos;
        last_y = y_pos;
        firstMouse = false;
    }

    f32 x_offset = x_pos - last_x;
    f32 y_offset = last_y - y_pos; // reversed since y-coordinates go from bottom to top
    last_x = x_pos;
    last_y = y_pos;

    f32 sensitivity = 0.1f; // change this value to your liking
    x_offset *= sensitivity;
    y_offset *= sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    vec3 front;
    front.x = cos(deg_to_rad(yaw)) * cos(deg_to_rad(pitch));
    front.y = sin(deg_to_rad(pitch));
    front.z = sin(deg_to_rad(yaw)) * cos(deg_to_rad(pitch));
    camera_front = front;

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
