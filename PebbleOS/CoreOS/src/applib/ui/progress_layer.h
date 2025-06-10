/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file progress_layer.h
 * @author Evan Stoddard
 * @brief Layer displaying a progress bar
 */

#ifndef progress_layer_h
#define progress_layer_h

#include "layer.h"

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
 * @brief Defintiion of progress bar layer
 */
typedef struct
{
    Layer layer;
    unsigned int progress_percent;
    GColor foreground_color;
    GColor background_color;
} ProgressLayer;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize progress bar layer
 *
 * @param progress_layer Pointer to progress layer
 * @param frame Pointer to frame of progress bar
 */
void progress_layer_init(ProgressLayer *progress_layer, const GRect *frame);

/**
 * @brief Deinitialize progress layer
 *
 * @param progress_layer Pointer to progress layer
 */
void progress_layer_deinit(ProgressLayer *progress_layer);

/**
 * @brief Set foreground of progress layer
 *
 * @param progress_layer Pointer to progress Layer
 * @param color Color to set foreground
 */
void progress_layer_set_foreground_color(ProgressLayer *progress_layer, GColor color);

/**
 * @brief Set background color of progress layer
 *
 * @param progress_layer Pointer to progress layer
 * @param color Color to set background
 */
void progress_layer_set_background_color(ProgressLayer *progress_layer, GColor color);

/**
 * @brief Convenience function to set progress percentage and mark layer as dirty
 *
 * @param progress_layer Pointer to progress layer
 * @param progress_percent Percentage of progres
 */
void progress_layer_set_progress(ProgressLayer *progress_layer, unsigned int progress_percent);

#ifdef __cplusplus
}
#endif
#endif /* progress_layer_h */
