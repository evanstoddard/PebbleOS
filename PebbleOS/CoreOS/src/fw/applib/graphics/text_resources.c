/*
 * Copyright (C) Ovyl
 */

/**
 * @file text_resources.c
 * @author Evan Stoddard
 * @brief
 */

#include "text_resources.h"

#include <stddef.h>

#include "fonts/font.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define HASH_TABLE_SIZE (255U)

#define HASH_TABLE_START_OFFSET (sizeof(FontMetaData))

#define OFFSET_TABLE_START_OFFSET (HASH_TABLE_START_OFFSET + (HASH_TABLE_SIZE + sizeof(FontHashTableEntry)))

/*****************************************************************************
 * Variables
 *****************************************************************************/

typedef struct __attribute__((packed)) OffsetTableEntry
{
    uint16_t codepoint;
    uint16_t offset_words;
} OffsetTableEntry;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param codepoint [TODO:parameter]
 * @return [TODO:return]
 */
inline uint8_t prv_hash_function(uint16_t codepoint)
{
    return codepoint % HASH_TABLE_SIZE;
}

/**
 * @brief [TODO:description]
 *
 * @param font [TODO:parameter]
 * @param table_offset [TODO:parameter]
 * @param codepoint [TODO:parameter]
 * @return [TODO:return]
 */
const GlyphData *prv_find_glyph_in_table(const FontMetaData *font, uint16_t table_offset, uint8_t table_length,
                                         uint16_t codepoint)
{
    GlyphData *glyph = NULL;

    void *offset_table_ptr = ((uint8_t *)font + sizeof(FontMetaData) + (sizeof(FontHashTableEntry) * 255));
    OffsetTableEntry *offset_table = (OffsetTableEntry *)offset_table_ptr;
    uint8_t *glyph_table = (uint8_t *)offset_table_ptr + (font->num_glyphs * sizeof(OffsetTableEntry));

    for (uint8_t i = table_offset; i < (table_offset + table_length); i++)
    {
        if (offset_table[i].codepoint == codepoint)
        {
            glyph = (GlyphData *)&glyph_table[offset_table[i].offset_words * 4];
            break;
        }
    }

    return glyph;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

const GlyphData *text_resources_get_glyph(uint16_t codepoint, FontMetaData const *font)
{
    uint8_t hash = prv_hash_function(codepoint);
    uint8_t *hash_table = (uint8_t *)font + sizeof(FontMetaData);
    FontHashTableEntry hash_entry = ((FontHashTableEntry *)hash_table)[hash];

    if (hash_entry.count == 0)
    {
        return NULL;
    }

    return prv_find_glyph_in_table(font, hash_entry.offset, hash_entry.count, codepoint);
}
