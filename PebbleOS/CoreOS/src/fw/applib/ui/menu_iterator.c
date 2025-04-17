/*
 * Copyright (C) Ovyl
 */

/**
 * @file menu_iterator.c
 * @author Evan Stoddard
 * @brief
 */

#include "menu_iterator.h"

#include <stddef.h>
#include <string.h>

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

void menu_iterator_init(MenuIterator *iterator, MenuLayer *menu_layer)
{
    memset(iterator, 0, sizeof(MenuIterator));

    iterator->layer = menu_layer;
    iterator->should_continue = true;
}

void menu_iterator_set_callbacks(MenuIterator *iterator, MenuIteratorCallbacks *callbacks)
{
    iterator->callbacks = *callbacks;
}

void menu_iterator_traverse_downward(MenuIterator *iterator)
{
    MenuLayer *layer = iterator->layer;
    uint16_t num_sections = 1;

    if (layer->callbacks.num_sections != NULL)
    {
        num_sections = layer->callbacks.num_sections(layer, NULL);
    }

    uint16_t *section = &iterator->cursor.index.section;
    uint16_t *row = &iterator->cursor.index.row;

    // Fugly
    for (*section = 0; *section < num_sections; (*section)++)
    {
        uint16_t rows = layer->callbacks.num_rows(layer, *section, NULL);

        // Also Fugly
        for (*row = 0; *row < rows; (*row)++)
        {
            MenuCellSpan *cursor = &iterator->cursor;

            if (iterator->callbacks.before_geometry != NULL)
            {
                iterator->callbacks.before_geometry(iterator);
            }

            // Determine the height of the cell at the current cursor
            cursor->size.h =
                (layer->callbacks.row_height != NULL) ? layer->callbacks.row_height(layer, &cursor->index, NULL) : 25;

            // Determine the height of the cell separator
            // TODO: Add callback...
            cursor->separator_height = 1;

            if (iterator->callbacks.after_geometry != NULL)
            {
                iterator->callbacks.after_geometry(iterator);
            }
        }
    }

    iterator->should_continue = false;
}

void menu_iterator_traverse_upward(MenuIterator *iterator);
