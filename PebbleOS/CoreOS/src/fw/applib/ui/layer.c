/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file layer.c
 * @author Evan Stoddard
 * @brief Implementation for base type defining UI elements
 */

#include "layer.h"

#include <stddef.h>
#include <string.h>

#include <zephyr/logging/log.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(ll_layer);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param stack [TODO:parameter]
 * @param stack_size [TODO:parameter]
 * @param current_depth [TODO:parameter]
 * @param descend [TODO:parameter]
 * @return [TODO:return]
 */
inline static Layer __attribute__((always_inline)) *
    prv_layer_tree_traverse_next(Layer *stack[], int const stack_size, uint8_t *current_depth, const bool descend)
{
    const Layer *top_of_stack = stack[*current_depth];

    // goto first child
    if (descend && top_of_stack->first_child)
    {
        if (*current_depth < stack_size - 1)
        {
            return stack[++(*current_depth)] = (Layer *)top_of_stack->first_child;
        }
        else
        {
        }
    }

    // no children, try next sibling
    if (top_of_stack->next_sibling)
    {
        return stack[*current_depth] = (Layer *)top_of_stack->next_sibling;
    }

    // there are no more siblings
    // continue with siblings of parents/grandparents
    while (*current_depth > 0)
    {
        (*current_depth)--;
        const Layer *sibling = (const Layer *)stack[*current_depth]->next_sibling;
        if (sibling)
        {
            return stack[*current_depth] = (Layer *)sibling;
        }
    }

    // no more siblings on root level of stack
    return NULL;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void layer_init(Layer *layer, GRect frame)
{
    memset(layer, 0, sizeof(Layer));

    layer->frame = frame;
    layer->bounds = GRect(0, 0, frame.size.w, frame.size.h);
    layer->update_proc = NULL;
}

struct Window *layer_get_window(Layer *layer)
{
    return layer->window;
}

void layer_render_tree(Layer *node, GContext *ctx)
{
    const GDrawState root_draw_state = ctx->draw_state;
    uint8_t current_depth = 0;

    Layer *stack[16] = {0};
    stack[0] = node;

    while (node)
    {
        bool descend = false;

        // call the current node's render procedure
        if (node->update_proc)
        {
            node->update_proc(node, ctx);
        }

        descend = true;

        node = prv_layer_tree_traverse_next(stack, 16, &current_depth, descend);

        ctx->draw_state = root_draw_state;
    }
}
