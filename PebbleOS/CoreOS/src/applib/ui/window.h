/*
 * Copyright (C) Ovyl
 */

/**
 * @file window.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef window_h
#define window_h

#include <stdbool.h>

#include "layer.h"

#include "graphics/gcontext.h"
#include "graphics/gtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef void (*WindowHandler)(struct Window *window);

/**
 * @typedef WindowHandlers
 * @brief Window handlers
 *
 */
typedef struct WindowHandlers
{
    WindowHandler load;
    WindowHandler appear;
    WindowHandler disappear;
    WindowHandler unload;
} WindowHandlers;

/**
 * @typedef Window
 * @brief [TODO:description]
 *
 */
typedef struct Window
{
    Layer layer;
    GColor background_color;

    WindowHandlers window_handlers;

    bool is_render_schedule : 1;
    bool on_screen : 1;
    bool is_loaded : 1;
    bool overrides_back_button : 1;
    bool is_fullscreen : 1;
    bool in_click_config_provider : 1;
    bool is_waiting_for_click_config : 1;
    bool is_click_configured : 1;
    bool is_transparent : 1;
    bool is_unfocusable : 1;

    const char *debug_name;
} Window;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param window [TODO:parameter]
 * @param debug_name [TODO:parameter]
 */
void window_init(Window *window, const char *debug_name);

/**
 * @brief [TODO:description]
 *
 * @param window [TODO:parameter]
 * @return [TODO:return]
 */
Layer *window_get_root_layer(const Window *window);

/**
 * @brief [TODO:description]
 *
 * @param window [TODO:parameter]
 * @param background_color [TODO:parameter]
 */
void window_set_background_color(Window *window, GColor background_color);

/**
 * @brief [TODO:description]
 *
 * @param window [TODO:parameter]
 * @param ctx [TODO:parameter]
 */
void window_render(Window *window, GContext *ctx);

/**
 * @brief [TODO:description]
 *
 * @param window [TODO:parameter]
 * @param on_screen [TODO:parameter]
 * @param call_handlers [TODO:parameter]
 */
void window_set_on_screen(Window *window, bool on_screen, bool call_handlers);

/**
 * @brief [TODO:description]
 *
 * @param window [TODO:parameter]
 */
void window_schedule_render(Window *window);

#ifdef __cplusplus
}
#endif
#endif /* window_h */
