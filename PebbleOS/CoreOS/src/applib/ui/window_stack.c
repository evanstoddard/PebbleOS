/*
 * Copyright (C) Ovyl
 */

/**
 * @file window_stack.c
 * @author Evan Stoddard
 * @brief
 */

#include "window_stack.h"

#include "memory/applib_malloc.h"

#include "util/list.h"
#include "window_stack_private.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Filter callback for determining if stack node contains pointer to requested window
 *
 * @param node Stack node to check
 * @param data Pointer to window to compare against
 * @return Returns true if stack node contains pointer to window
 */
static bool prv_stack_filter_callback(ListNode *node, void *data)
{
    WindowStackNode *stack_node = (WindowStackNode *)node;

    Window *window = (Window *)data;

    if (stack_node->window == window)
    {
        return true;
    }

    return false;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void window_stack_init(WindowStack *window_stack)
{
    memset(window_stack, 0, sizeof(WindowStack));

    list_init(&window_stack->active_windows);
    list_init(&window_stack->removed_windows);
}

Window *window_stack_get_top_window(WindowStack *window_stack)
{
    if (window_stack == NULL)
    {
        return NULL;
    }

    WindowStackNode *node = (WindowStackNode *)list_get_head(&window_stack->active_windows);

    if (node == NULL)
    {
        return NULL;
    }

    return node->window;
}

void window_stack_push(WindowStack *window_stack, Window *window, bool animated)
{
    if (window_stack->push_lock == true)
    {
        return;
    }

    if (window_stack_count(window_stack) >= WINDOW_STACK_ITEMS_MAX)
    {
        return;
    }

    // Check if window has been removed previously and push back onto stack
    WindowStackNode *node =
        (WindowStackNode *)list_find(&window_stack->removed_windows, prv_stack_filter_callback, window);

    if (node != NULL)
    {
        list_remove(&window_stack->removed_windows, (ListNode *)node);

        // TODO: Should this actually be freed? Original code didn't but it didn't seem right...
        applib_free(node);
    }

    // Check if window already in stack and remove so it can be reinserted
    node = NULL;
    node = (WindowStackNode *)list_find(&window_stack->active_windows, prv_stack_filter_callback, window);

    if (node != NULL)
    {
        list_remove(&window_stack->active_windows, (ListNode *)node);
    }
    else
    {
        node = applib_type_malloc(WindowStackNode);

        list_node_init((ListNode *)node);

        node->window = window;
    }

    list_append(&window_stack->active_windows, (ListNode *)node);
}

Window *window_stack_pop(WindowStack *window_stack, bool animated)
{
    WindowStackNode *node = (WindowStackNode *)list_pop_tail(&window_stack->active_windows);

    if (node == NULL)
    {
        return NULL;
    }

    list_append(&window_stack->removed_windows, (ListNode *)node);

    WindowStackNode *new_node = (WindowStackNode *)list_get_tail(&window_stack->active_windows);

    window_set_on_screen(node->window, false, true);

    if (new_node != NULL)
    {
        window_set_on_screen(node->window, true, true);
    }

    return node->window;
}

void window_stack_pop_all(WindowStack *window_stack, const bool animated)
{
    window_stack_lock_push(window_stack);
}

bool window_stack_remove(Window *window, bool animated)
{
    return false;
}

bool window_stack_contains_window(WindowStack *window_stack, Window *window)
{
    return false;
}

uint32_t window_stack_count(WindowStack *window_stack)
{
    return list_count(&window_stack->active_windows);
}

void window_stack_lock_push(WindowStack *window_stack)
{
    window_stack->push_lock = true;
}

void window_stack_unlock_push(WindowStack *window_stack)
{
    window_stack->push_lock = false;
}
