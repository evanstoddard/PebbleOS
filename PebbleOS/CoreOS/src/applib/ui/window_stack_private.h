/*
 * Copyright (C) Ovyl
 */

/**
 * @file window_stack_private.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef window_stack_private_h
#define window_stack_private_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include <zephyr/sys/slist.h>

#include "ui/window.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define WINDOW_STACK_ITEMS_MAX (16)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef WindowStackNode
 * @brief Window stack node
 *
 */
typedef struct WindowStackNode
{
    // This must be the first property
    sys_snode_t node;

    Window *window;
} WindowStackNode;

/**
 * @typedef WindowStack
 * @brief Window stack
 *
 */
typedef struct WindowStack
{
    sys_slist_t active_windows;
    sys_slist_t removed_windows;

    WindowStackNode *last_top_node;

    bool push_lock;
} WindowStack;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize window stack
 *
 * @param window_stack Pointer to window stack
 */
void window_stack_init(WindowStack *window_stack);

#ifdef __cplusplus
}
#endif
#endif /* window_stack_private_h */
