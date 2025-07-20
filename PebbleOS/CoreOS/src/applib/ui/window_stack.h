/*
 * Copyright (C) Ovyl
 */

/**
 * @file window_stack.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef window_stack_h
#define window_stack_h

#ifdef __cplusplus
extern "C" {
#endif

#include "window_stack_private.h"

#include <stdbool.h>
#include <stdint.h>

#include "ui/window.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Get top window of stack
 *
 * @param window_stack Pointer to window stack
 * @return Window at top or NULL
 */
Window *window_stack_get_top_window(WindowStack *window_stack);

/**
 * @brief Push window to stack
 *
 * @param window_stack Pointer to window stack
 * @param window Pointer to window
 * @param animated If push should be animated (Currently ignored)
 */
void window_stack_push(WindowStack *window_stack, Window *window, bool animated);

/**
 * @brief Pop window off stack
 *
 * @param window_stack Pointer to window stack
 * @param animated If pop should be animated (currently ignored)
 * @return Pointer to popped window
 */
Window *window_stack_pop(WindowStack *window_stack, bool animated);

/**
 * @brief Pop all windows off stack
 *
 * @param window_stack Pointer to window stack
 * @param animated If pops should be animated (currently ignored)
 */
void window_stack_pop_all(WindowStack *window_stack, const bool animated);

/**
 * @brief Remove window from stack
 *
 * @param window Pointer to window
 * @param animated If removal should be animated (currently ignored)
 * @return If successfully removed from stack
 */
bool window_stack_remove(Window *window, bool animated);

/**
 * @brief If stack contains window
 *
 * @param window_stack Pointer to window stack
 * @param window Window to check for
 * @return Returns true if window is in stack
 */
bool window_stack_contains_window(WindowStack *window_stack, Window *window);

/**
 * @brief Number of windows in stack
 *
 * @param window_stack Pointer to window stack
 * @return Number of windows in stack
 */
uint32_t window_stack_count(WindowStack *window_stack);

/**
 * @brief Prevent windows from being pushed to stack
 *
 * @param window_stack Pointer to window stack
 */
void window_stack_lock_push(WindowStack *window_stack);

/**
 * @brief Allow windows to be pushed to stack
 *
 * @param window_stack Pointer to window stack
 */
void window_stack_unlock_push(WindowStack *window_stack);

#ifdef __cplusplus
}
#endif
#endif /* window_stack_h */
