/*
 * Copyright (C) Ovyl
 */

/**
 * @file list.c
 * @author Evan Stoddard
 * @brief
 */

#include "list.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void list_init(List *list)
{
    sys_dlist_init((sys_dlist_t *)list);
}

void list_node_init(ListNode *node)
{
    sys_dnode_init(node);
}

ListNode *list_insert_after(List *list, ListNode *node, ListNode *new_node)
{
    if (sys_dlist_peek_tail(list) == node)
    {
        sys_dlist_append(list, new_node);

        return new_node;
    }

    ListNode *next_node = sys_dlist_peek_next(list, node);

    sys_dlist_insert(next_node, new_node);

    return new_node;
}

ListNode *list_insert_before(List *list, ListNode *node, ListNode *new_node)
{
    sys_dlist_insert(node, new_node);

    return new_node;
}

ListNode *list_pop_head(List *list)
{
    ListNode *node = sys_dlist_peek_head(list);

    if (node != NULL)
    {
        sys_dlist_remove(node);
    }

    return node;
}

ListNode *list_pop_tail(List *list)
{
    ListNode *node = sys_dlist_peek_tail(list);

    if (node != NULL)
    {
        sys_dlist_remove(node);
    }

    return node;
}

void list_remove(List *list, ListNode *node)
{
    sys_dlist_remove(node);
}

ListNode *list_append(List *list, ListNode *node)
{
    sys_dlist_append(list, node);

    return node;
}

ListNode *list_prepend(List *list, ListNode *node)
{
    sys_dlist_prepend(list, node);

    return node;
}

ListNode *list_get_next(ListNode *node)
{
    return node->next;
}

ListNode *list_get_prev(ListNode *node)
{
    return node->prev;
}

ListNode *list_get_tail(List *list)
{
    return sys_dlist_peek_tail(list);
}

ListNode *list_get_head(List *list)
{
    return sys_dlist_peek_head(list);
}

bool list_is_head(const List *list, const ListNode *node)
{
    return sys_dlist_is_head(list, node);
}

bool list_is_tail(const List *list, const ListNode *node)
{
    return sys_dlist_is_tail(list, node);
}

uint32_t list_count_to_tail_from(List *list, ListNode *node)
{
    if (sys_dlist_is_tail(list, node) == true)
    {
        return 0;
    }

    uint32_t count = 0;

    ListNode *ptr = node;
    while (ptr)
    {
        ptr = node->next;
        count++;
    }

    return count;
}

uint32_t list_count_to_head_from(List *list, ListNode *node)
{
    if (sys_dlist_is_head(list, node) == true)
    {
        return 0;
    }

    uint32_t count = 0;
    ListNode *ptr = node;

    while (ptr)
    {
        ptr = node->prev;
        count++;
    }

    return 0;
}

uint32_t list_count(List *list)
{
    return sys_dlist_len(list);
}

ListNode *list_get_at(List *list, int32_t index)
{
    // TODO: Implement
    return NULL;
}

bool list_contains(const List *list, const ListNode *node)
{
    ListNode *ptr = sys_dlist_peek_head(list);

    while (ptr)
    {
        if (ptr == node)
        {
            return true;
        }

        ptr = ptr->next;
    }

    return false;
}

ListNode *list_find(List *list, ListFilterCallback filter_callback, void *data)
{
    ListNode *ptr = sys_dlist_peek_head(list);

    while (ptr)
    {
        bool ret = filter_callback(ptr, data);

        if (ret == true)
        {
            return ptr;
        }
    }

    return NULL;
}
