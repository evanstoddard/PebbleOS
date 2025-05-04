/*
 * Copyright (C) Ovyl
 */

/**
 * @file menu_iterator.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef menu_iterator_h
#define menu_iterator_h

#include <stdbool.h>

#include "menu_cell.h"
#include "menu_layer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @class MenuIterator
 * @brief [TODO:description]
 *
 */
struct MenuIterator;

/**
 * @typedef MenuIteratorCallbacks
 * @brief [TODO:description]
 *
 */
typedef struct MenuIteratorCallbacks
{
    void (*before_geometry)(struct MenuIterator *iterator);
    void (*after_geometry)(struct MenuIterator *iterator);
} MenuIteratorCallbacks;

/**
 * @typedef MenuIterator
 * @brief [TODO:description]
 *
 */
typedef struct MenuIterator
{
    struct MenuLayer *layer;
    MenuCellSpan cursor;
    bool should_continue;
    MenuIteratorCallbacks callbacks;
    int16_t cell_height;
} MenuIterator;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param menu_layer [TODO:parameter]
 */
void menu_iterator_init(MenuIterator *iterator, MenuLayer *menu_layer);

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param callbacks [TODO:parameter]
 */
void menu_iterator_set_callbacks(MenuIterator *iterator, MenuIteratorCallbacks *callbacks);

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 */
void menu_iterator_traverse_downward(MenuIterator *iterator);

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 */
void menu_iterator_traverse_upward(MenuIterator *iterator);

#ifdef __cplusplus
}
#endif
#endif /* menu_iterator_h */
