/*
 * Copyright (C) Ovyl
 */

/**
 * @file menu_cell.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef menu_cell_h
#define menu_cell_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "graphics/gtypes.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef MenuIndex
 * @brief [TODO:description]
 *
 */
typedef struct MenuIndex
{
    uint16_t section;
    uint16_t row;
} MenuIndex;

/**
 * @typedef MenuCellSpan
 * @brief [TODO:description]
 *
 */
typedef struct MenuCellSpan
{
    int16_t y;
    int16_t h;
    int16_t separator_height;
    MenuIndex index;
} MenuCellSpan;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* menu_cell_h */
