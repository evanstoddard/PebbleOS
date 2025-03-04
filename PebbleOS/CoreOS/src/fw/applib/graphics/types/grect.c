/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file grect.c
 * @author Evan Stoddard
 * @brief Implementation of GRect function
 */

#include "grect.h"

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
 * Private Functions
 *****************************************************************************/

 /*****************************************************************************
 * Public Functions
 *****************************************************************************/

 bool grect_is_empty(const GRect *rect) {
    return (rect->size.h == 0 || rect->size.w == 0);
 }

GRect grect_union(const GRect *r1, const GRect *r2) {
    if (grect_is_empty(r1) && grect_is_empty(r2)) {
        return GRectZero;
    }

    if (grect_is_empty(r1)) {
        return *r2;
    }

    if (grect_is_empty(r2)) {
        return *r1;
    }

    GRect out = GRectZero;

    int16_t min_x = (r1->origin.x < r2->origin.x) ? r1->origin.x : r2->origin.x;
    int16_t max_x = ((r1->origin.x + r1->size.w) > (r2->origin.x + r2->size.w)) ? (r1->origin.x +r1->size.w) : (r2->origin.x + r2->size.w);

    int16_t min_y = (r1->origin.y < r2->origin.y) ? r1->origin.y : r2->origin.y;
    int16_t max_y = ((r1->origin.y + r1->size.h) > (r2->origin.y + r2->size.h)) ? (r1->origin.y + r1->size.h) : (r2->origin.y + r2->size.h);

    out.origin.x = min_x;
    out.origin.y = min_y;

    out.size.w = max_x - min_x;
    out.size.h = max_y - min_y;

    return out;
}

bool grect_contains_point(const GRect *rect, const GPoint *point) {
    int16_t min_x = rect->origin.x;
    int16_t max_x = rect->origin.x + rect->size.w;

    int16_t min_y = rect->origin.y;
    int16_t max_y = rect->origin.y + rect->size.h;

    if (point->x < min_x || point->x >= max_x) {
        return false;
    }

    if (point->y < min_y || point->y >= max_y) {
        return false;
    }

    return true;
}