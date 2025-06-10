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
#include "resource/resource_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define FONT_HASH_TABLE_LENGTH 255U

#define MAX_OFFSET_TABLE_LENGTH 128U

#define CACHE_GLYPH_SIZE_BYTES 256U

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
    uint8_t *data;
} GlyphData;

/**
 * @typedef FontResource
 * @brief [TODO:description]
 *
 */
typedef struct FontResource
{
    ResourceStorageEntry resource_entry;
    FontMetaData meta_data;
} FontResource;

/**
 * @typedef FontInfo
 * @brief [TODO:description]
 *
 */
typedef struct FontInfo
{
    bool loaded;
    FontResource resource;
} FontInfo;

/**
 * @typedef FontCache
 * @brief [TODO:description]
 *
 */
typedef struct FontCache
{
    FontResource *resource;

    int32_t offset_table_id;
    uint32_t offset_table_length;

    OffsetTableEntry offset_table_cache[MAX_OFFSET_TABLE_LENGTH];

    uint8_t glyph_data_cache[CACHE_GLYPH_SIZE_BYTES];
    GlyphData last_glyph;
} FontCache;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param font_info [TODO:parameter]
 * @param app_num [TODO:parameter]
 * @param resource_id [TODO:parameter]
 * @return [TODO:return]
 */
bool text_resources_init_font(FontInfo *font_info, uint32_t app_num, uint32_t resource_id);

/**
 * @brief [TODO:description]
 *
 * @param font_cache [TODO:parameter]
 * @param font [TODO:parameter]
 * @param codepoint [TODO:parameter]
 * @return [TODO:return]
 */
const GlyphData *text_resources_get_glyph(FontCache *font_cache, GFont font, uint16_t codepoint);

#ifdef __cplusplus
}
#endif
#endif /* text_resources_h */
