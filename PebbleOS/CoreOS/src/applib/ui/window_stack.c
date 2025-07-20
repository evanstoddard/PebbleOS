/*
 * Copyright (C) Ovyl
 */

/**
 * @file window_stack.c
 * @author Evan Stoddard
 * @brief
 */

#include "window_stack.h"

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

/*****************************************************************************
 * Functions
 *****************************************************************************/

void window_stack_init(WindowStack *window_stack)
{
    memset(window_stack, 0, sizeof(WindowStack));

    sys_slist_init(&window_stack->active_windows);
    sys_slist_init(&window_stack->removed_windows);
}

Window *window_stack_get_top_window(WindowStack *window_stack)
{
    return NULL;
}

void window_stack_push(WindowStack *window_stack, Window *window, bool animated)
{
}

Window *window_stack_pop(WindowStack *window_stack, bool animated)
{
    return NULL;
}

void window_stack_pop_all(WindowStack *window_stack, const bool animated)
{
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
    return 0;
}

void window_stack_lock_push(WindowStack *window_stack)
{
}

void window_stack_unlock_push(WindowStack *window_stack)
{
}
