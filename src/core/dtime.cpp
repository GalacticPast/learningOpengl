#include "dtime.hpp"
#include "platform/platform.hpp"

void clock_start(clock_context *clock_state)
{
    clock_state->start_time = platform_get_absolute_time();
    clock_state->elapsed = 0.0f;
}
void clock_update(clock_context *clock_state)
{
    if (clock_state->start_time != 0)
    {
        clock_state->elapsed = platform_get_absolute_time() - clock_state->start_time;
    }
}
void clock_stop(clock_context *clock_state)
{
    clock_state->start_time = 0;
}
