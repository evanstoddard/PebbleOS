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

#include "ui/window.h"

#include "util/list.h"

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
    // Must be first property
    ListNode node;

    Window *window;
} WindowStackNode;

/**
 * @typedef WindowStack
 * @brief Window stack
 *
 */
typedef struct WindowStack
{
    List active_windows;
    List removed_windows;

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
