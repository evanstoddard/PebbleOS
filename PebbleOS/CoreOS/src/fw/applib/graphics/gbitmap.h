/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file gbitmap.h
 * @author Evan Stoddard
 * @brief Definion and functions for GBitmap
 */

#ifndef gbitmap_h
#define gbitmap_h

#include <stdint.h>

#include "framebuffer.h"
#include "gtypes.h"

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
 * @brief Definition of GBitmap type
 *
 */
typedef struct GBitmap {
  void *addr;
  uint16_t row_size_bytes;
  GRect bounds;
} GBitmap;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Create bitmap from framebuffer
 *
 * @param fb Pointer to framebuffer
 * @param size Size of bitmap
 * @return GBitmap Resulting bitmap
 */
GBitmap framebuffer_get_as_bitmap(FrameBuffer *fb, const GSize *size);

#ifdef __cplusplus
}
#endif
#endif /* gbitmap_h */
