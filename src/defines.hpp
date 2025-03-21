#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#ifdef __MING32__
#define WINDOWS
#endif

#ifdef __linux__
#define LINUX
#endif
