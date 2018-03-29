//========================================================================
// GLFW 3.3 XKB - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2018 Kovid Goyal <kovid@kovidgoyal.net>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#include <string.h>
#include <stdlib.h>
#include "internal.h"
#include "xkb_glfw.h"


#ifdef _GLFW_X11

GLFWbool
glfw_xkb_set_x11_events_mask(void) {
    if (!XkbSelectEvents(_glfw.x11.display, XkbUseCoreKbd, XkbNewKeyboardNotifyMask | XkbMapNotifyMask | XkbStateNotifyMask, XkbNewKeyboardNotifyMask | XkbMapNotifyMask | XkbStateNotifyMask)) {
        _glfwInputError(GLFW_PLATFORM_ERROR, "Failed to set XKB events mask");
        return GLFW_FALSE;
    }
    return GLFW_TRUE;
}

GLFWbool
glfw_xkb_update_x11_keyboard_id(_GLFWXKBData *xkb) {
    xkb->keyboard_device_id = -1;
    xcb_connection_t* conn = XGetXCBConnection(_glfw.x11.display);
    if (!conn) {
        _glfwInputError(GLFW_PLATFORM_ERROR, "X11: Failed to retrieve XCB connection");
        return GLFW_FALSE;
    }

    xkb->keyboard_device_id = xkb_x11_get_core_keyboard_device_id(conn);
    if (xkb->keyboard_device_id == -1) {
        _glfwInputError(GLFW_PLATFORM_ERROR, "X11: Failed to retrieve core keyboard device id");
        return GLFW_FALSE;
    }
    return GLFW_TRUE;
}

#define xkb_glfw_load_keymap(keymap, ...) {\
    xcb_connection_t* conn = XGetXCBConnection(_glfw.x11.display); \
    if (conn) keymap = xkb_x11_keymap_new_from_device(xkb->context, conn, xkb->keyboard_device_id, XKB_KEYMAP_COMPILE_NO_FLAGS); \
}

#define xkb_glfw_load_state(keymap, state, ...) {\
    xcb_connection_t* conn = XGetXCBConnection(_glfw.x11.display); \
    if (conn) state = xkb_x11_state_new_from_device(keymap, conn, xkb->keyboard_device_id); \
}

#else

#define xkb_glfw_load_keymap(keymap, map_str) keymap = xkb_keymap_new_from_string(xkb->context, map_str, XKB_KEYMAP_FORMAT_TEXT_V1, 0);
#define xkb_glfw_load_state(keymap, state, ...) state = xkb_state_new(keymap);

#endif

void
glfw_xkb_release(_GLFWXKBData *xkb) {
    if (xkb->composeState) {
        xkb_compose_state_unref(xkb->composeState);
        xkb->composeState = NULL;
    }
    if (xkb->keymap) {
        xkb_keymap_unref(xkb->keymap);
        xkb->keymap = NULL;
    }
    if (xkb->state) {
        xkb_state_unref(xkb->state);
        xkb->state = NULL;
    }
    if (xkb->context) {
        xkb_context_unref(xkb->context);
        xkb->context = NULL;
    }
}

GLFWbool
glfw_xkb_create_context(_GLFWXKBData *xkb) {
    xkb->context = xkb_context_new(0);
    if (!xkb->context)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Failed to initialize XKB context");
        return GLFW_FALSE;
    }
    int scancode;
    memset(xkb->keycodes, -1, sizeof(xkb->keycodes));
    memset(xkb->scancodes, -1, sizeof(xkb->scancodes));

    xkb->keycodes[KEY_GRAVE]      = GLFW_KEY_GRAVE_ACCENT;
    xkb->keycodes[KEY_1]          = GLFW_KEY_1;
    xkb->keycodes[KEY_2]          = GLFW_KEY_2;
    xkb->keycodes[KEY_3]          = GLFW_KEY_3;
    xkb->keycodes[KEY_4]          = GLFW_KEY_4;
    xkb->keycodes[KEY_5]          = GLFW_KEY_5;
    xkb->keycodes[KEY_6]          = GLFW_KEY_6;
    xkb->keycodes[KEY_7]          = GLFW_KEY_7;
    xkb->keycodes[KEY_8]          = GLFW_KEY_8;
    xkb->keycodes[KEY_9]          = GLFW_KEY_9;
    xkb->keycodes[KEY_0]          = GLFW_KEY_0;
    xkb->keycodes[KEY_SPACE]      = GLFW_KEY_SPACE;
    xkb->keycodes[KEY_MINUS]      = GLFW_KEY_MINUS;
    xkb->keycodes[KEY_EQUAL]      = GLFW_KEY_EQUAL;
    xkb->keycodes[KEY_Q]          = GLFW_KEY_Q;
    xkb->keycodes[KEY_W]          = GLFW_KEY_W;
    xkb->keycodes[KEY_E]          = GLFW_KEY_E;
    xkb->keycodes[KEY_R]          = GLFW_KEY_R;
    xkb->keycodes[KEY_T]          = GLFW_KEY_T;
    xkb->keycodes[KEY_Y]          = GLFW_KEY_Y;
    xkb->keycodes[KEY_U]          = GLFW_KEY_U;
    xkb->keycodes[KEY_I]          = GLFW_KEY_I;
    xkb->keycodes[KEY_O]          = GLFW_KEY_O;
    xkb->keycodes[KEY_P]          = GLFW_KEY_P;
    xkb->keycodes[KEY_LEFTBRACE]  = GLFW_KEY_LEFT_BRACKET;
    xkb->keycodes[KEY_RIGHTBRACE] = GLFW_KEY_RIGHT_BRACKET;
    xkb->keycodes[KEY_A]          = GLFW_KEY_A;
    xkb->keycodes[KEY_S]          = GLFW_KEY_S;
    xkb->keycodes[KEY_D]          = GLFW_KEY_D;
    xkb->keycodes[KEY_F]          = GLFW_KEY_F;
    xkb->keycodes[KEY_G]          = GLFW_KEY_G;
    xkb->keycodes[KEY_H]          = GLFW_KEY_H;
    xkb->keycodes[KEY_J]          = GLFW_KEY_J;
    xkb->keycodes[KEY_K]          = GLFW_KEY_K;
    xkb->keycodes[KEY_L]          = GLFW_KEY_L;
    xkb->keycodes[KEY_SEMICOLON]  = GLFW_KEY_SEMICOLON;
    xkb->keycodes[KEY_APOSTROPHE] = GLFW_KEY_APOSTROPHE;
    xkb->keycodes[KEY_Z]          = GLFW_KEY_Z;
    xkb->keycodes[KEY_X]          = GLFW_KEY_X;
    xkb->keycodes[KEY_C]          = GLFW_KEY_C;
    xkb->keycodes[KEY_V]          = GLFW_KEY_V;
    xkb->keycodes[KEY_B]          = GLFW_KEY_B;
    xkb->keycodes[KEY_N]          = GLFW_KEY_N;
    xkb->keycodes[KEY_M]          = GLFW_KEY_M;
    xkb->keycodes[KEY_COMMA]      = GLFW_KEY_COMMA;
    xkb->keycodes[KEY_DOT]        = GLFW_KEY_PERIOD;
    xkb->keycodes[KEY_SLASH]      = GLFW_KEY_SLASH;
    xkb->keycodes[KEY_BACKSLASH]  = GLFW_KEY_BACKSLASH;
    xkb->keycodes[KEY_ESC]        = GLFW_KEY_ESCAPE;
    xkb->keycodes[KEY_TAB]        = GLFW_KEY_TAB;
    xkb->keycodes[KEY_LEFTSHIFT]  = GLFW_KEY_LEFT_SHIFT;
    xkb->keycodes[KEY_RIGHTSHIFT] = GLFW_KEY_RIGHT_SHIFT;
    xkb->keycodes[KEY_LEFTCTRL]   = GLFW_KEY_LEFT_CONTROL;
    xkb->keycodes[KEY_RIGHTCTRL]  = GLFW_KEY_RIGHT_CONTROL;
    xkb->keycodes[KEY_LEFTALT]    = GLFW_KEY_LEFT_ALT;
    xkb->keycodes[KEY_RIGHTALT]   = GLFW_KEY_RIGHT_ALT;
    xkb->keycodes[KEY_LEFTMETA]   = GLFW_KEY_LEFT_SUPER;
    xkb->keycodes[KEY_RIGHTMETA]  = GLFW_KEY_RIGHT_SUPER;
    xkb->keycodes[KEY_MENU]       = GLFW_KEY_MENU;
    xkb->keycodes[KEY_NUMLOCK]    = GLFW_KEY_NUM_LOCK;
    xkb->keycodes[KEY_CAPSLOCK]   = GLFW_KEY_CAPS_LOCK;
    xkb->keycodes[KEY_PRINT]      = GLFW_KEY_PRINT_SCREEN;
    xkb->keycodes[KEY_SCROLLLOCK] = GLFW_KEY_SCROLL_LOCK;
    xkb->keycodes[KEY_PAUSE]      = GLFW_KEY_PAUSE;
    xkb->keycodes[KEY_DELETE]     = GLFW_KEY_DELETE;
    xkb->keycodes[KEY_BACKSPACE]  = GLFW_KEY_BACKSPACE;
    xkb->keycodes[KEY_ENTER]      = GLFW_KEY_ENTER;
    xkb->keycodes[KEY_HOME]       = GLFW_KEY_HOME;
    xkb->keycodes[KEY_END]        = GLFW_KEY_END;
    xkb->keycodes[KEY_PAGEUP]     = GLFW_KEY_PAGE_UP;
    xkb->keycodes[KEY_PAGEDOWN]   = GLFW_KEY_PAGE_DOWN;
    xkb->keycodes[KEY_INSERT]     = GLFW_KEY_INSERT;
    xkb->keycodes[KEY_LEFT]       = GLFW_KEY_LEFT;
    xkb->keycodes[KEY_RIGHT]      = GLFW_KEY_RIGHT;
    xkb->keycodes[KEY_DOWN]       = GLFW_KEY_DOWN;
    xkb->keycodes[KEY_UP]         = GLFW_KEY_UP;
    xkb->keycodes[KEY_F1]         = GLFW_KEY_F1;
    xkb->keycodes[KEY_F2]         = GLFW_KEY_F2;
    xkb->keycodes[KEY_F3]         = GLFW_KEY_F3;
    xkb->keycodes[KEY_F4]         = GLFW_KEY_F4;
    xkb->keycodes[KEY_F5]         = GLFW_KEY_F5;
    xkb->keycodes[KEY_F6]         = GLFW_KEY_F6;
    xkb->keycodes[KEY_F7]         = GLFW_KEY_F7;
    xkb->keycodes[KEY_F8]         = GLFW_KEY_F8;
    xkb->keycodes[KEY_F9]         = GLFW_KEY_F9;
    xkb->keycodes[KEY_F10]        = GLFW_KEY_F10;
    xkb->keycodes[KEY_F11]        = GLFW_KEY_F11;
    xkb->keycodes[KEY_F12]        = GLFW_KEY_F12;
    xkb->keycodes[KEY_F13]        = GLFW_KEY_F13;
    xkb->keycodes[KEY_F14]        = GLFW_KEY_F14;
    xkb->keycodes[KEY_F15]        = GLFW_KEY_F15;
    xkb->keycodes[KEY_F16]        = GLFW_KEY_F16;
    xkb->keycodes[KEY_F17]        = GLFW_KEY_F17;
    xkb->keycodes[KEY_F18]        = GLFW_KEY_F18;
    xkb->keycodes[KEY_F19]        = GLFW_KEY_F19;
    xkb->keycodes[KEY_F20]        = GLFW_KEY_F20;
    xkb->keycodes[KEY_F21]        = GLFW_KEY_F21;
    xkb->keycodes[KEY_F22]        = GLFW_KEY_F22;
    xkb->keycodes[KEY_F23]        = GLFW_KEY_F23;
    xkb->keycodes[KEY_F24]        = GLFW_KEY_F24;
    xkb->keycodes[KEY_KPSLASH]    = GLFW_KEY_KP_DIVIDE;
    xkb->keycodes[KEY_KPDOT]      = GLFW_KEY_KP_MULTIPLY;
    xkb->keycodes[KEY_KPMINUS]    = GLFW_KEY_KP_SUBTRACT;
    xkb->keycodes[KEY_KPPLUS]     = GLFW_KEY_KP_ADD;
    xkb->keycodes[KEY_KP0]        = GLFW_KEY_KP_0;
    xkb->keycodes[KEY_KP1]        = GLFW_KEY_KP_1;
    xkb->keycodes[KEY_KP2]        = GLFW_KEY_KP_2;
    xkb->keycodes[KEY_KP3]        = GLFW_KEY_KP_3;
    xkb->keycodes[KEY_KP4]        = GLFW_KEY_KP_4;
    xkb->keycodes[KEY_KP5]        = GLFW_KEY_KP_5;
    xkb->keycodes[KEY_KP6]        = GLFW_KEY_KP_6;
    xkb->keycodes[KEY_KP7]        = GLFW_KEY_KP_7;
    xkb->keycodes[KEY_KP8]        = GLFW_KEY_KP_8;
    xkb->keycodes[KEY_KP9]        = GLFW_KEY_KP_9;
    xkb->keycodes[KEY_KPCOMMA]    = GLFW_KEY_KP_DECIMAL;
    xkb->keycodes[KEY_KPEQUAL]    = GLFW_KEY_KP_EQUAL;
    xkb->keycodes[KEY_KPENTER]    = GLFW_KEY_KP_ENTER;

    for (scancode = 0;  scancode < 256;  scancode++)
    {
        if (xkb->keycodes[scancode] > 0)
            xkb->scancodes[xkb->keycodes[scancode]] = scancode;
    }
    return GLFW_TRUE;
}

GLFWbool
glfw_xkb_compile_keymap(_GLFWXKBData *xkb, const char *map_str) {
    const char* locale = NULL;
    struct xkb_state* state = NULL;
    struct xkb_keymap* keymap = NULL;
    struct xkb_compose_table* compose_table = NULL;
    struct xkb_compose_state* compose_state = NULL;
    (void)(map_str);  // not needed on X11

    xkb_glfw_load_keymap(keymap, map_str);
    if (!keymap) _glfwInputError(GLFW_PLATFORM_ERROR, "Failed to compile XKB keymap");
    else {
        xkb_glfw_load_state(keymap, state);
        if (!state) {
            _glfwInputError(GLFW_PLATFORM_ERROR, "Failed to create XKB state");
            xkb_keymap_unref(keymap); keymap = NULL;
        } else {
            /* Look up the preferred locale, falling back to "C" as default. */
            locale = getenv("LC_ALL");
            if (!locale) locale = getenv("LC_CTYPE");
            if (!locale) locale = getenv("LANG");
            if (!locale) locale = "C";
            compose_table = xkb_compose_table_new_from_locale(xkb->context, locale, XKB_COMPOSE_COMPILE_NO_FLAGS);
            if (!compose_table) {
                _glfwInputError(GLFW_PLATFORM_ERROR, "Failed to create XKB compose table");
                xkb_keymap_unref(keymap); keymap = NULL;
                xkb_state_unref(state); state = NULL;
            } else {
                compose_state = xkb_compose_state_new(compose_table, XKB_COMPOSE_STATE_NO_FLAGS);
                xkb_compose_table_unref(compose_table); compose_table = NULL;
                if (!compose_state) {
                    _glfwInputError(GLFW_PLATFORM_ERROR, "Failed to create XKB compose state");
                    xkb_keymap_unref(keymap); keymap = NULL;
                    xkb_state_unref(state); state = NULL;
                }
            }
        }
    }
    if (keymap && state && compose_state) {
        if (xkb->composeState) xkb_compose_state_unref(xkb->composeState);
        xkb->composeState = compose_state;
        if (xkb->keymap) xkb_keymap_unref(xkb->keymap);
        xkb->keymap = keymap;
        if (xkb->state) xkb_state_unref(xkb->state);
        xkb->state = state;
    }
    if (xkb->keymap) {
        xkb->controlMask = 1 << xkb_keymap_mod_get_index(xkb->keymap, "Control");
        xkb->altMask = 1 << xkb_keymap_mod_get_index(xkb->keymap, "Mod1");
        xkb->shiftMask = 1 << xkb_keymap_mod_get_index(xkb->keymap, "Shift");
        xkb->superMask = 1 << xkb_keymap_mod_get_index(xkb->keymap, "Mod4");
        xkb->capsLockMask = 1 << xkb_keymap_mod_get_index(xkb->keymap, "Lock");
        xkb->numLockMask = 1 << xkb_keymap_mod_get_index(xkb->keymap, "Mod2");
    }
    return GLFW_TRUE;
}

void
glfw_xkb_update_modifiers(_GLFWXKBData *xkb, unsigned int depressed, unsigned int latched, unsigned int locked, unsigned int group) {
    xkb_mod_mask_t mask;
    unsigned int modifiers = 0;
    if (!xkb->keymap) return;
    xkb_state_update_mask(xkb->state, depressed, latched, locked, 0, 0, group);
    mask = xkb_state_serialize_mods(xkb->state, XKB_STATE_MODS_DEPRESSED | XKB_STATE_LAYOUT_DEPRESSED | XKB_STATE_MODS_LATCHED | XKB_STATE_LAYOUT_LATCHED);
    if (mask & xkb->controlMask) modifiers |= GLFW_MOD_CONTROL;
    if (mask & xkb->altMask) modifiers |= GLFW_MOD_ALT;
    if (mask & xkb->shiftMask) modifiers |= GLFW_MOD_SHIFT;
    if (mask & xkb->superMask) modifiers |= GLFW_MOD_SUPER;
    if (mask & xkb->capsLockMask) modifiers |= GLFW_MOD_CAPS_LOCK;
    if (mask & xkb->numLockMask) modifiers |= GLFW_MOD_NUM_LOCK;
    xkb->modifiers = modifiers;
}

int
glfw_xkb_to_glfw_key_code(_GLFWXKBData *xkb, unsigned int key) {
    return ((key < sizeof(xkb->keycodes) / sizeof(xkb->keycodes[0])) ? xkb->keycodes[key] : GLFW_KEY_UNKNOWN);
}

GLFWbool
glfw_xkb_should_repeat(_GLFWXKBData *xkb, xkb_keycode_t scancode) {
#ifndef _GLFW_X11
    scancode += 8;
#endif
    return xkb_keymap_key_repeats(xkb->keymap, scancode);
}

static xkb_keysym_t
compose_symbol(_GLFWXKBData *xkb, xkb_keysym_t sym) {
    if (sym == XKB_KEY_NoSymbol || !xkb->composeState) return sym;
    if (xkb_compose_state_feed(xkb->composeState, sym) != XKB_COMPOSE_FEED_ACCEPTED) return sym;
    switch (xkb_compose_state_get_status(xkb->composeState)) {
        case XKB_COMPOSE_COMPOSED:
            return xkb_compose_state_get_one_sym(xkb->composeState);
        case XKB_COMPOSE_COMPOSING:
        case XKB_COMPOSE_CANCELLED:
            return XKB_KEY_NoSymbol;
        case XKB_COMPOSE_NOTHING:
        default:
            return sym;
    }
}


void
glfw_xkb_handle_key_event(_GLFWwindow *window, _GLFWXKBData *xkb, int key, xkb_keycode_t scancode, int action, int *codepoint, int *plain) {
    const xkb_keysym_t *syms;
    _glfwInputKey(window, key, scancode, action, xkb->modifiers);
    *codepoint = -1;

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        xkb_keycode_t code_for_sym = scancode;
#ifndef _GLFW_X11
    code_for_sym += 8;
#endif
        int num_syms = xkb_state_key_get_syms(xkb->state, code_for_sym, &syms);
        if (num_syms == 1) {
            xkb_keysym_t sym = compose_symbol(xkb, syms[0]);
            *codepoint = _glfwKeySym2Unicode(sym);
            if (*codepoint != -1) {
                const int mods = xkb->modifiers;
                *plain = !(mods & (GLFW_MOD_CONTROL | GLFW_MOD_ALT));
                _glfwInputChar(window, *codepoint, mods, *plain);
            }
        }
    }
}
