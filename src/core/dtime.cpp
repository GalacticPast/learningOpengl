#include "dtime.hpp"

static clock_context static_clock_state;

void clock_initialize(clock_context *clock_state)
{
    clock_state->start_clock = std::chrono::steady_clock::now();
    static_clock_state = *clock_state;
}

f64 get_time()
{
    std::chrono::steady_clock::time_point end_clock = std::chrono::steady_clock::now();
    f64 duration = static_cast<f64>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - static_clock_state.start_clock).count());
    return duration;
}
