/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file gpoint.h
 * @author Evan Stoddard
 * @brief Definition of GPoint
 */

#ifndef gpoint_h
#define gpoint_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/**
 * @brief Macro for creating a GPoint
 * 
 */
#define GPoint(x, y) (GPoint){(x), (y)}

/**
 * @brief Macro for greating a GPoint at the origin
 * 
 */
#define GPointZero GPoint(0, 0)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @brief Defintion of GPoint
 * 
 */
typedef struct GPoint {
    int16_t x;
    int16_t y;
} GPoint;

/**
 * @brief Work-around to allow GPoint macro to be used in conjunction with
 *        GPoint typedef.
 * 
 */
typedef GPoint GPointReturn;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* gpoint_h */