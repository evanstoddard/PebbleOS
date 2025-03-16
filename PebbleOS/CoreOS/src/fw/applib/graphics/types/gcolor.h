/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file gcolor.h
 * @author Evan Stoddard
 * @brief 
 */

#ifndef gcolor_h
#define gcolor_h

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
 * @brief Definition of color
 * 
 */
typedef enum {
    GColorBlack = 0,
    GColorWhite = 1,
    GColorClear = ~0
} GColor;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* gcolor_h */