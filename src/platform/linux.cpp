#include "defines.hpp"
#include "platform.hpp"

#include "core/event.hpp"
#include "core/input.hpp"

#include <iostream>
#include <string.h>

#ifdef LINUX
#ifdef PLATFORM_LINUX_WAYLAND

#if PLATFORM_LINUX_WAYLAND

//
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "wayland/xdg-shell-client-protocol.h"
// opengl
#include <wayland-egl.h>

#include <EGL/egl.h>

#include <GLES2/gl2.h>

/* Wayland code */
typedef struct internal_state
{
    /* Globals */
    struct wl_display    *wl_display;
    struct wl_registry   *wl_registry;
    struct wl_shm        *wl_shm;
    struct wl_seat       *wl_seat;
    struct wl_compositor *wl_compositor;
    struct xdg_wm_base   *xdg_wm_base;

    /* Objects */
    struct wl_surface   *wl_surface;
    struct xdg_surface  *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;

    /* input */
    struct wl_keyboard *wl_keyboard;
    struct wl_mouse    *wl_mouse;

    /* EGL stuff */
    struct wl_egl_window *egl_window;
    EGLDisplay            egl_display;
    EGLConfig             egl_config;
    EGLContext            egl_context;
    EGLSurface            egl_surface;

    s32 x;
    s32 y;
    s32 width;
    s32 height;

} internal_state;

keys translate_keycode(u32 key);
// extern "C" inline void xdg_toplevel_set_title(struct xdg_toplevel, const char *application_name);
// extern "C" inline void xdg_toplevel_set_app_id(struct xdg_toplevel, const char *application_name);

static void wl_keyboard_keymap(void *data, struct wl_keyboard *wl_keyboard, u32 format, s32 fd, u32 size)
{
}

static void wl_keyboard_enter(void *data, struct wl_keyboard *wl_keyboard, u32 serial, struct wl_surface *surface,
                              struct wl_array *keys)
{
    DEBUG("Keyboard in scope");
}

static void wl_keyboard_key(void *data, struct wl_keyboard *wl_keyboard, u32 serial, u32 time, u32 key, u32 state)
{

    keys code = translate_keycode(key);

    if (code == KEY_ESCAPE)
    {
        std::cout << "Apllication exit code recieved." << std::endl;

        event_context context = {};
        event_fire(EVENT_CODE_APPLICATION_QUIT, nullptr, context);
    }

    input_process_key(code, (bool)state);
}

static void wl_keyboard_leave(void *data, struct wl_keyboard *wl_keyboard, u32 serial, struct wl_surface *surface)
{
    DEBUG("Mouse not in scope");
}

static void wl_keyboard_modifiers(void *data, struct wl_keyboard *wl_keyboard, u32 serial, u32 mods_depressed,
                                  u32 mods_latched, u32 mods_locked, u32 group)
{
}

static void wl_keyboard_repeat_info(void *data, struct wl_keyboard *wl_keyboard, s32 rate, s32 delay)
{
}

static const struct wl_keyboard_listener wl_keyboard_listener = {
    .keymap = wl_keyboard_keymap,
    .enter = wl_keyboard_enter,
    .leave = wl_keyboard_leave,
    .key = wl_keyboard_key,
    .modifiers = wl_keyboard_modifiers,
    .repeat_info = wl_keyboard_repeat_info,
};
//

static void wl_seat_capabilites(void *data, struct wl_seat *wl_seat, u32 capabilities)
{
    internal_state *state = (internal_state *)data;

    // TODO: mouse events
    //

    bool have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

    if (have_keyboard && state->wl_keyboard == NULL)
    {
        state->wl_keyboard = wl_seat_get_keyboard(state->wl_seat);
        wl_keyboard_add_listener(state->wl_keyboard, &wl_keyboard_listener, state);
    }
    else if (!have_keyboard && state->wl_keyboard != NULL)
    {
        wl_keyboard_release(state->wl_keyboard);
        state->wl_keyboard = NULL;
    }
}
static void wl_seat_name(void *data, struct wl_seat *wl_seat, const char *name)
{
}

struct wl_seat_listener wl_seat_listener = {.capabilities = wl_seat_capabilites, .name = wl_seat_name};

// actual surface
static void xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel, s32 width, s32 height,
                                   struct wl_array *states)
{
    if (width == 0 || height == 0)
    {
        return;
    }
    internal_state *state = (internal_state *)data;
    state->width = width;
    state->height = height;

    // event_context context = {};
    // context.data.u32[0] = width;
    // context.data.u32[1] = height;
    // event_fire(ON_APPLICATION_RESIZE, context);
}

static void xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel)
{
}

static void toplevel_configure_bounds(void *data, struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height)
{
}

static void toplevel_wm_capabilites(void *data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities)
{
}

struct xdg_toplevel_listener xdg_toplevel_listener = {.configure = xdg_toplevel_configure,
                                                      .close = xdg_toplevel_close,
                                                      .configure_bounds = toplevel_configure_bounds,
                                                      .wm_capabilities = toplevel_wm_capabilites};
//

static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, u32 serial)
{
    internal_state *state = (internal_state *)data;

    xdg_surface_ack_configure(xdg_surface, serial);
    wl_surface_commit(state->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

// shell
static void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, u32 serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};
//

static void registry_global(void *data, struct wl_registry *wl_registry, u32 name, const char *interface, u32 version)
{
    internal_state *state = (internal_state *)data;

    // std::cout << interface << " " << version << std::endl;

    if (!strcmp(interface, wl_compositor_interface.name))
    {
        state->wl_compositor =
            (struct wl_compositor *)wl_registry_bind(wl_registry, name, &wl_compositor_interface, version);
    }
    else if (!strcmp(interface, xdg_wm_base_interface.name))
    {
        state->xdg_wm_base = (struct xdg_wm_base *)wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, version);
        xdg_wm_base_add_listener(state->xdg_wm_base, &xdg_wm_base_listener, state);
    }
    else if (!strcmp(interface, wl_seat_interface.name))
    {
        state->wl_seat = (struct wl_seat *)wl_registry_bind(wl_registry, name, &wl_seat_interface, version);
        wl_seat_add_listener(state->wl_seat, &wl_seat_listener, state);
    }
}

static void registry_global_remove(void *data, struct wl_registry *wl_registry, u32 name)
{
    /* This space deliberately left blank */
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

bool platform_startup(platform_state *plat_state, std::string application_name, s32 x, s32 y, s32 width, s32 height)
{
    INFO("Initializing linux-Wayland platform...");

    plat_state->internal_state = (internal_state *)malloc(sizeof(internal_state));
    internal_state *state = (internal_state *)plat_state->internal_state;
    memset(state, 0, sizeof(internal_state));

    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;

    state->wl_display = wl_display_connect(NULL);
    if (!state->wl_display)
    {
        return false;
    }

    state->wl_registry = wl_display_get_registry(state->wl_display);
    if (!state->wl_registry)
    {
        return false;
    }
    wl_registry_add_listener(state->wl_registry, &wl_registry_listener, state);

    wl_display_roundtrip(state->wl_display);

    state->wl_surface = wl_compositor_create_surface(state->wl_compositor);
    if (!state->wl_surface)
    {
        return false;
    }

    state->xdg_surface = xdg_wm_base_get_xdg_surface(state->xdg_wm_base, state->wl_surface);
    if (!state->xdg_surface)
    {
        return false;
    }

    xdg_surface_add_listener(state->xdg_surface, &xdg_surface_listener, state);

    state->xdg_toplevel = xdg_surface_get_toplevel(state->xdg_surface);

    if (!state->xdg_toplevel)
    {
        return false;
    }

    xdg_toplevel_add_listener(state->xdg_toplevel, &xdg_toplevel_listener, state);

    xdg_toplevel_set_title(state->xdg_toplevel, application_name.c_str());
    xdg_toplevel_set_app_id(state->xdg_toplevel, application_name.c_str());

    wl_surface_commit(state->wl_surface);

    INFO("Linux-Wayland platform initialized");

    return true;
}

bool platform_pump_messages(platform_state *plat_state)
{
    internal_state *state = (internal_state *)plat_state->internal_state;

    s32 result = wl_display_dispatch(state->wl_display);

    return result == -1 ? false : true;
}

void platform_shutdown(platform_state *plat_state)
{
    internal_state *state = (internal_state *)plat_state->internal_state;

    eglDestroySurface(state->egl_display, state->egl_surface);
    eglDestroyContext(state->egl_display, state->egl_context);
    wl_egl_window_destroy(state->egl_window);

    xdg_toplevel_destroy(state->xdg_toplevel);
    xdg_surface_destroy(state->xdg_surface);
    wl_surface_destroy(state->wl_surface);
    wl_display_disconnect(state->wl_display);
}

bool init_openGL(platform_state *plat_state)
{
    internal_state *state = (internal_state *)plat_state->internal_state;

    EGLint major = 2;
    EGLint minor = 0;
    EGLint num_configs;

    EGLint attributes[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE};

    EGLint context_attributes[] = {};

    state->egl_window = wl_egl_window_create(state->wl_surface, state->width, state->height);

    if (state->egl_window == EGL_NO_SURFACE)
    {
        return false;
    }

    state->egl_display = eglGetDisplay((EGLNativeDisplayType)state->wl_display);

    if (state->egl_display == EGL_NO_DISPLAY)
    {
        ERROR("Couldnt get EGL display.");
        return false;
    }

    if (eglInitialize(state->egl_display, &major, &minor) != EGL_TRUE)
    {
        ERROR("Couldn't initialize EGL");
        return false;
    }

    if (eglChooseConfig(state->egl_display, attributes, &state->egl_config, 1, &num_configs) != EGL_TRUE)
    {
        ERROR("Couldn't find matching EGL config");
        return false;
    }

    state->egl_surface =
        eglCreateWindowSurface(state->egl_display, state->egl_config, (EGLNativeWindowType)state->egl_window, NULL);

    if (state->egl_surface == EGL_NO_SURFACE)
    {
        ERROR("Couldn't create EGL surface");
        return false;
    }

    state->egl_context = eglCreateContext(state->egl_display, state->egl_config, EGL_NO_CONTEXT, NULL);
    if (state->egl_context == EGL_NO_CONTEXT)
    {
        ERROR("Couldn't create EGL context");
        return false;
    }

    if (!eglMakeCurrent(state->egl_display, state->egl_surface, state->egl_surface, state->egl_context))
    {
        ERROR("Couldn't make EGL context current");
        return false;
    }

    INFO("EGL initialized");
    return true;
}

void platform_swap_buffers(platform_state *plat_state)
{
    internal_state *state = (internal_state *)plat_state->internal_state;

    eglSwapBuffers(state->egl_display, state->egl_surface);
}

keys translate_keycode(u32 wl_keycode)
{
    switch (wl_keycode)
    {
        case 1: {
            return KEY_ESCAPE;
        }
        break;
        case 2: {
            return KEYS_MAX_KEYS;
            // return KEY_NUMPAD1;
        }
        break;
        case 3: {
            return KEYS_MAX_KEYS;
            // return KEY_NUMPAD2;
        }
        break;
        case 4: {
            return KEYS_MAX_KEYS;
            // return KEY_NUMPAD3;
        }
        break;
        case 5: {
            return KEYS_MAX_KEYS;
            // return KEY_NUMPAD4;
        }
        break;
        case 6: {
            return KEYS_MAX_KEYS;
            // return KEY_NUMPAD5;
        }
        break;
        case 7: {
            return KEYS_MAX_KEYS;
            // return KEY_NUMPAD6;
        }
        break;
        case 8: {
            return KEYS_MAX_KEYS;
            // return KEY_NUMPAD7;
        }
        break;
        case 9: {
            return KEYS_MAX_KEYS;
            // return KEY_NUMPAD8;
        }
        break;
        case 10: {
            return KEYS_MAX_KEYS;
            // return KEY_NUMPAD9;
        }
        break;
        case 11: {
            return KEYS_MAX_KEYS;
            // return KEY_NUMPADMAX_KEYS;
        }
        break;
        case 12: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 13: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 14: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 15: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 16: {
            return KEY_Q;
        }
        break;
        case 17: {
            return KEY_W;
        }
        break;
        case 18: {
            return KEY_E;
        }
        break;
        case 19: {
            return KEY_R;
        }
        break;
        case 20: {
            return KEY_T;
        }
        break;
        case 21: {
            return KEY_Y;
        }
        break;
        case 22: {
            return KEY_U;
        }
        break;
        case 23: {
            return KEY_I;
        }
        break;
        case 24: {
            return KEY_O;
        }
        break;
        case 25: {
            return KEY_P;
        }
        break;
        case 26: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 27: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 28: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 29: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 30: {
            return KEY_A;
        }
        break;
        case 31: {
            return KEY_S;
        }
        break;
        case 32: {
            return KEY_D;
        }
        break;
        case 33: {
            return KEY_F;
        }
        break;
        case 34: {
            return KEY_G;
        }
        break;
        case 35: {
            return KEY_H;
        }
        break;
        case 36: {
            return KEY_J;
        }
        break;
        case 37: {
            return KEY_K;
        }
        break;
        case 38: {
            return KEY_L;
        }
        break;
        case 39: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 40: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 41: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 42: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 43: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 44: {
            return KEY_Z;
        }
        break;
        case 45: {
            return KEY_X;
        }
        break;
        case 46: {
            return KEY_C;
        }
        break;
        case 47: {
            return KEY_V;
        }
        break;
        case 48: {
            return KEY_B;
        }
        break;
        case 49: {
            return KEY_N;
        }
        break;
        case 50: {
            return KEY_M;
        }
        break;
        case 51: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 52: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 53: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 54: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 55: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 56: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 57: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 58: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 59: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 60: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 61: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 62: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 63: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 64: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 65: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 66: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 67: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 68: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 69: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 70: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 71: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 72: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 73: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 74: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 75: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 76: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 77: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 78: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 79: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 80: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 81: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 82: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 83: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 84: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 85: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 86: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 87: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 88: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 89: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 90: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 91: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 92: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 93: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 94: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 95: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 96: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 97: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 98: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 99: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 100: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 101: {
            return KEYS_MAX_KEYS;
        }
        break;
        case 102: {
            return KEYS_MAX_KEYS;
        }
        break;
    }
    return KEYS_MAX_KEYS;
}

#endif
#endif

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

#endif
