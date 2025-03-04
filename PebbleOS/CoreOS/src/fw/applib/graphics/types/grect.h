/*
 * Copyright (C) Ovyl
 */

/**
 * @file grect.h
 * @author Evan Stoddard
 * @brief Definition and functions for GRect
 */

#ifndef grect_h
#define grect_h

#include <stdbool.h>

#include "gpoint.h"
#include "gsize.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/**
 * @brief Macro for creating new GRect
 * 
 */
#define GRect(x, y, w, h) ((GRect) {{(x), (y)}, {(w), (h)}})



/**
 * @brief Macro for greating a zero-ed rect
 * 
 */
#define GRectZero GRect(0, 0, 0, 0)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @brief Definition of GRect
 * 
 */
typedef struct GRect {
    GPoint origin;
    GSize size;
} GRect;

/**
 * @brief Workaround to get GRect macro to play nicely with GRect typedef
 * 
 */
typedef GRect GRectReturn;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Returns whether rect is empty (either w or h is 0)
 * 
 * @param rect Pointer to rect
 * @return true Rect is empty
 * @return false Rect is not empty
 */
bool grect_is_empty(const GRect *rect);

/**
 * @brief Return smallest GRect containing r1 & r2
 * 
 * @param r1 First rect
 * @param r2 Second rect
 * @return GRect Resultant rect
 */
GRect grect_union(const GRect *r1, const GRect *r2);

/**
 * @brief Returns whether a rect contains a point
 * 
 * @param rect Pointer to rect
 * @param point Pointer to point
 * @return true Rect contains point
 * @return false Rect does not contain point
 */
bool grect_contains_point(const GRect *rect, const GPoint *point);

#ifdef __cplusplus
}
#endif
#endif /* grect_h */