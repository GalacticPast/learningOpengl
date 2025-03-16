#include "core/event.hpp"
#include "platform/platform.hpp"
#include <iostream>

static bool is_running;
bool        shutdown(u16 code, void *sender, void *listener_inst, event_context data);

int main(void)
{
    platform_state plat_state = {};
    std::string    application_name = "learnOpengl";
    s32            x = 0;
    s32            y = 0;
    s32            width = 1270;
    s32            height = 800;
    is_running = true;

    event_initialize();
    event_register(EVENT_CODE_APPLICATION_QUIT, NULL, shutdown);

    bool result = platform_startup(&plat_state, application_name, x, y, width, height);
    if (result == false)
    {
        std::cout << "well it didnt work." << std::endl;
        return 0;
    }
    result = init_openGL(&plat_state);

    while (is_running)
    {
        if (platform_pump_messages(&plat_state))
        {
            draw(&plat_state);
        }
    }

    platform_shutdown(&plat_state);
    event_shutdown();
}
bool shutdown(u16 code, void *sender, void *listener_inst, event_context data)
{
    is_running = false;
    return true;
}
