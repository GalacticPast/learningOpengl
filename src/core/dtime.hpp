#pragma once
#include "defines.hpp"

struct clock_context
{
    // the start time of the clock
    f64 start_time;
    // how much time in elapsed since the clock has started
    f64 elapsed;
};

void clock_start(clock_context *clock_state);
void clock_update(clock_context *clock_state);
void clock_stop(clock_context *clock_state);
