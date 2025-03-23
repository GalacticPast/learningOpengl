#pragma once
#include "defines.hpp"

#include <chrono>
#include <ctime>

struct clock_context
{
    std::chrono::steady_clock::time_point start_clock;
};

void clock_initialize(clock_context *clock_state);
f64 get_time_nano();
f64 get_time_milli();
f64 get_time_sec();
