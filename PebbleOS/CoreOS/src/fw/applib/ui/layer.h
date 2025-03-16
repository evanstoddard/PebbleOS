/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file layer.h
 * @author Evan Stoddard
 * @brief Definitions for base type for UI elements
 */

#ifndef layer_h
#define layer_h

#include "graphics/gcontext.h"
#include "graphics/gtypes.h"

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
 * @brief Forward definition of base Layer type.
 *
 */
struct Layer;

typedef void (*LayerUpdateProc)(struct Layer *layer, GContext *ctx);

/**
 * @typedef Layer
 * @brief Definition of Layer base type
 *
 */
typedef struct Layer
{
    GRect bounds;
    GRect frame;

    LayerUpdateProc update_proc;
} Layer;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize layer
 *
 * @param layer Pointer to layer
 * @param frame Frame of layer
 */
void layer_init(Layer *layer, GRect frame);

#ifdef __cplusplus
}
#endif
#endif /* layer_h */
