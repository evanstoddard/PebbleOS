/*
 * Copyright (C) Ovyl
 */

/**
 * @file text_resources.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef text_resources_h
#define text_resources_h

#include <stdint.h>

#include "fonts/font.h"

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
 * @typedef GlyphHeaderData
 * @brief Definition of glyph header
 *
 */
typedef struct __attribute__((packed)) GlyphHeaderData
{
    uint8_t width;
    uint8_t height;
    int8_t offset_top;
    int8_t offset_left;
    uint8_t unused[3];
    uint8_t horizontal_advance;
} GlyphHeaderData;

/**
 * @typedef GlyphData
 * @brief Definition of glyph
 *
 */
typedef struct __attribute__((packed)) GlyphData
{
    GlyphHeaderData header;
    uint8_t data[];
} GlyphData;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

const GlyphData *text_resources_get_glyph(uint16_t codepoint, FontMetaData const *font);

#ifdef __cplusplus
}
#endif
#endif /* text_resources_h */
