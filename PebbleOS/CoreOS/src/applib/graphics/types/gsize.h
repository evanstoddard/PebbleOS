/*
 * Copyright (C) Ovyl
 */

/**
 * @file gsize.h
 * @author Evan Stoddard
 * @brief Defintion of GSize and its functions
 */

#ifndef gsize_h
#define gsize_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/**
 * @brief Macro to create new GSize
 * 
 */
#define GSize(w, h) ((GSize) {(w), (h)});

/**
 * @brief Macro to create GSize of 0,0
 * 
 */
#define GSizeZero GSize(0, 0)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @brief Definition of GSize
 * 
 */
typedef struct GSize {
    int16_t w;
    int16_t h;
} GSize;

/**
 * @brief Workaround to get GSize macro to play nice with GSize typedef
 * 
 */
typedef GSize GSizeReturn;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* gsize_h */