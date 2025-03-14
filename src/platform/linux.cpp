#include "defines.hpp"
#include "platform.hpp"
#include <GL/gl.h>
#include <iostream>
#include <string.h>

#ifdef LINUX
#ifdef PLATFORM_LINUX_WAYLAND

#if PLATFORM_LINUX_WAYLAND

//
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "wayland/xdg-shell-client-protocol.h"

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

} internal_state;

// u32 translate_keycode(u32 key);
// extern "C" inline void xdg_toplevel_set_title(struct xdg_toplevel, const char *application_name);
// extern "C" inline void xdg_toplevel_set_app_id(struct xdg_toplevel, const char *application_name);

static void wl_keyboard_keymap(void *data, struct wl_keyboard *wl_keyboard, u32 format, s32 fd, u32 size)
{
    // struct internal_state *state = data;
}

static void wl_keyboard_enter(void *data, struct wl_keyboard *wl_keyboard, u32 serial, struct wl_surface *surface,
                              struct wl_array *keys)
{
    //    DEBUG("Keyboard in scope");
}

static void wl_keyboard_key(void *data, struct wl_keyboard *wl_keyboard, u32 serial, u32 time, u32 key, u32 state)
{
    // struct internal_state *internal_state = data;

    //    keys code = translate_keycode(key);

    //   input_process_key(code, (b8)state);
}

static void wl_keyboard_leave(void *data, struct wl_keyboard *wl_keyboard, u32 serial, struct wl_surface *surface)
{
    //  DEBUG("Mouse not in scope");
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
    // event_context context = {};
    // context.data.u32[0] = width;
    // context.data.u32[1] = height;
    // event_fire(ON_APPLICATION_RESIZE, context);
}

static void xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel)
{
}
struct xdg_toplevel_listener xdg_toplevel_listener = {.configure = xdg_toplevel_configure, .close = xdg_toplevel_close};
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

    if (strcmp(interface, wl_compositor_interface.name))
    {
        state->wl_compositor = (struct wl_compositor *)wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4);
    }
    else if (strcmp(interface, xdg_wm_base_interface.name))
    {
        state->xdg_wm_base = (struct xdg_wm_base *)wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(state->xdg_wm_base, &xdg_wm_base_listener, state);
    }
    else if (strcmp(interface, wl_seat_interface.name))
    {
        state->wl_seat = (struct wl_seat *)wl_registry_bind(wl_registry, name, &wl_seat_interface, 1);
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
    // INFO("Initializing linux-Wayland platform...");
    plat_state->internal_state = (internal_state *)malloc(sizeof(internal_state));
    internal_state *state = (internal_state *)plat_state->internal_state;

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

    // INFO("Linux-Wayland platform initialized");

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

    xdg_toplevel_destroy(state->xdg_toplevel);
    xdg_surface_destroy(state->xdg_surface);
    wl_surface_destroy(state->wl_surface);
    wl_display_disconnect(state->wl_display);
}

// u32 translate_keycode(u32 wl_keycode)
//{
//     switch (wl_keycode)
//     {
//         case 1: {
//             return KEY_ESCAPE;
//         }
//         break;
//         case 2: {
//             return MAX_KEYS;
//             // return KEY_NUMPAD1;
//         }
//         break;
//         case 3: {
//             return MAX_KEYS;
//             // return KEY_NUMPAD2;
//         }
//         break;
//         case 4: {
//             return MAX_KEYS;
//             // return KEY_NUMPAD3;
//         }
//         break;
//         case 5: {
//             return MAX_KEYS;
//             // return KEY_NUMPAD4;
//         }
//         break;
//         case 6: {
//             return MAX_KEYS;
//             // return KEY_NUMPAD5;
//         }
//         break;
//         case 7: {
//             return MAX_KEYS;
//             // return KEY_NUMPAD6;
//         }
//         break;
//         case 8: {
//             return MAX_KEYS;
//             // return KEY_NUMPAD7;
//         }
//         break;
//         case 9: {
//             return MAX_KEYS;
//             // return KEY_NUMPAD8;
//         }
//         break;
//         case 10: {
//             return MAX_KEYS;
//             // return KEY_NUMPAD9;
//         }
//         break;
//         case 11: {
//             return MAX_KEYS;
//             // return KEY_NUMPADMAX_KEYS;
//         }
//         break;
//         case 12: {
//             return MAX_KEYS;
//         }
//         break;
//         case 13: {
//             return MAX_KEYS;
//         }
//         break;
//         case 14: {
//             return MAX_KEYS;
//         }
//         break;
//         case 15: {
//             return MAX_KEYS;
//         }
//         break;
//         case 16: {
//             return KEY_Q;
//         }
//         break;
//         case 17: {
//             return KEY_W;
//         }
//         break;
//         case 18: {
//             return KEY_E;
//         }
//         break;
//         case 19: {
//             return KEY_R;
//         }
//         break;
//         case 20: {
//             return KEY_T;
//         }
//         break;
//         case 21: {
//             return KEY_Y;
//         }
//         break;
//         case 22: {
//             return KEY_U;
//         }
//         break;
//         case 23: {
//             return KEY_I;
//         }
//         break;
//         case 24: {
//             return KEY_O;
//         }
//         break;
//         case 25: {
//             return KEY_P;
//         }
//         break;
//         case 26: {
//             return MAX_KEYS;
//         }
//         break;
//         case 27: {
//             return MAX_KEYS;
//         }
//         break;
//         case 28: {
//             return MAX_KEYS;
//         }
//         break;
//         case 29: {
//             return MAX_KEYS;
//         }
//         break;
//         case 30: {
//             return KEY_A;
//         }
//         break;
//         case 31: {
//             return KEY_S;
//         }
//         break;
//         case 32: {
//             return KEY_D;
//         }
//         break;
//         case 33: {
//             return KEY_F;
//         }
//         break;
//         case 34: {
//             return KEY_G;
//         }
//         break;
//         case 35: {
//             return KEY_H;
//         }
//         break;
//         case 36: {
//             return KEY_J;
//         }
//         break;
//         case 37: {
//             return KEY_K;
//         }
//         break;
//         case 38: {
//             return KEY_L;
//         }
//         break;
//         case 39: {
//             return MAX_KEYS;
//         }
//         break;
//         case 40: {
//             return MAX_KEYS;
//         }
//         break;
//         case 41: {
//             return MAX_KEYS;
//         }
//         break;
//         case 42: {
//             return MAX_KEYS;
//         }
//         break;
//         case 43: {
//             return MAX_KEYS;
//         }
//         break;
//         case 44: {
//             return KEY_Z;
//         }
//         break;
//         case 45: {
//             return KEY_X;
//         }
//         break;
//         case 46: {
//             return KEY_C;
//         }
//         break;
//         case 47: {
//             return KEY_V;
//         }
//         break;
//         case 48: {
//             return KEY_B;
//         }
//         break;
//         case 49: {
//             return KEY_N;
//         }
//         break;
//         case 50: {
//             return KEY_M;
//         }
//         break;
//         case 51: {
//             return MAX_KEYS;
//         }
//         break;
//         case 52: {
//             return MAX_KEYS;
//         }
//         break;
//         case 53: {
//             return MAX_KEYS;
//         }
//         break;
//         case 54: {
//             return MAX_KEYS;
//         }
//         break;
//         case 55: {
//             return MAX_KEYS;
//         }
//         break;
//         case 56: {
//             return MAX_KEYS;
//         }
//         break;
//         case 57: {
//             return MAX_KEYS;
//         }
//         break;
//         case 58: {
//             return MAX_KEYS;
//         }
//         break;
//         case 59: {
//             return MAX_KEYS;
//         }
//         break;
//         case 60: {
//             return MAX_KEYS;
//         }
//         break;
//         case 61: {
//             return MAX_KEYS;
//         }
//         break;
//         case 62: {
//             return MAX_KEYS;
//         }
//         break;
//         case 63: {
//             return MAX_KEYS;
//         }
//         break;
//         case 64: {
//             return MAX_KEYS;
//         }
//         break;
//         case 65: {
//             return MAX_KEYS;
//         }
//         break;
//         case 66: {
//             return MAX_KEYS;
//         }
//         break;
//         case 67: {
//             return MAX_KEYS;
//         }
//         break;
//         case 68: {
//             return MAX_KEYS;
//         }
//         break;
//         case 69: {
//             return MAX_KEYS;
//         }
//         break;
//         case 70: {
//             return MAX_KEYS;
//         }
//         break;
//         case 71: {
//             return MAX_KEYS;
//         }
//         break;
//         case 72: {
//             return MAX_KEYS;
//         }
//         break;
//         case 73: {
//             return MAX_KEYS;
//         }
//         break;
//         case 74: {
//             return MAX_KEYS;
//         }
//         break;
//         case 75: {
//             return MAX_KEYS;
//         }
//         break;
//         case 76: {
//             return MAX_KEYS;
//         }
//         break;
//         case 77: {
//             return MAX_KEYS;
//         }
//         break;
//         case 78: {
//             return MAX_KEYS;
//         }
//         break;
//         case 79: {
//             return MAX_KEYS;
//         }
//         break;
//         case 80: {
//             return MAX_KEYS;
//         }
//         break;
//         case 81: {
//             return MAX_KEYS;
//         }
//         break;
//         case 82: {
//             return MAX_KEYS;
//         }
//         break;
//         case 83: {
//             return MAX_KEYS;
//         }
//         break;
//         case 84: {
//             return MAX_KEYS;
//         }
//         break;
//         case 85: {
//             return MAX_KEYS;
//         }
//         break;
//         case 86: {
//             return MAX_KEYS;
//         }
//         break;
//         case 87: {
//             return MAX_KEYS;
//         }
//         break;
//         case 88: {
//             return MAX_KEYS;
//         }
//         break;
//         case 89: {
//             return MAX_KEYS;
//         }
//         break;
//         case 90: {
//             return MAX_KEYS;
//         }
//         break;
//         case 91: {
//             return MAX_KEYS;
//         }
//         break;
//         case 92: {
//             return MAX_KEYS;
//         }
//         break;
//         case 93: {
//             return MAX_KEYS;
//         }
//         break;
//         case 94: {
//             return MAX_KEYS;
//         }
//         break;
//         case 95: {
//             return MAX_KEYS;
//         }
//         break;
//         case 96: {
//             return MAX_KEYS;
//         }
//         break;
//         case 97: {
//             return MAX_KEYS;
//         }
//         break;
//         case 98: {
//             return MAX_KEYS;
//         }
//         break;
//         case 99: {
//             return MAX_KEYS;
//         }
//         break;
//         case 100: {
//             return MAX_KEYS;
//         }
//         break;
//         case 101: {
//             return MAX_KEYS;
//         }
//         break;
//         case 102: {
//             return MAX_KEYS;
//         }
//         break;
//     }
//     return MAX_KEYS;
// }

#endif
#endif

#endif
