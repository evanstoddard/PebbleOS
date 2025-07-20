/*
 * Copyright (C) Ovyl
 */

/**
 * @file list.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef list_h
#define list_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/sys/dlist.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef sys_dlist_t List;

typedef sys_dnode_t ListNode;

typedef bool (*ListFilterCallback)(ListNode *found_node, void *data);

typedef bool (*ListForEachCallback)(ListNode *node, void *context);

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

void list_init(List *list);

void list_node_init(ListNode *node);

ListNode *list_insert_after(List *list, ListNode *node, ListNode *new_node);

ListNode *list_insert_before(List *list, ListNode *node, ListNode *new_node);

ListNode *list_pop_head(List *list);

ListNode *list_pop_tail(List *list);

void list_remove(List *list, ListNode *node);

ListNode *list_append(List *list, ListNode *node);

ListNode *list_prepend(List *list, ListNode *node);

ListNode *list_get_next(ListNode *node);

ListNode *list_get_prev(ListNode *node);

ListNode *list_get_tail(List *list);

ListNode *list_get_head(List *List);

bool list_is_head(const List *list, const ListNode *node);

bool list_is_tail(const List *list, const ListNode *node);

uint32_t list_count_to_tail_from(List *list, ListNode *node);

uint32_t list_count_to_head_from(List *list, ListNode *node);

uint32_t list_count(List *list);

ListNode *list_get_at(List *list, int32_t index);

bool list_contains(const List *list, const ListNode *node);

ListNode *list_find(List *list, ListFilterCallback filter_callback, void *data);

#ifdef __cplusplus
}
#endif
#endif /* list_h */
