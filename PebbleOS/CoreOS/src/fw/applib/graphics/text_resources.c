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
#include "resource/resource_storage.h"

#include <zephyr/logging/log.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define HASH_TABLE_START_OFFSET (sizeof(FontMetaData))

#define OFFSET_TABLE_START_OFFSET (HASH_TABLE_START_OFFSET + (FONT_HASH_TABLE_LENGTH * sizeof(FontHashTableEntry)))

LOG_MODULE_REGISTER(text_resources, LOG_LEVEL_DBG);

/*****************************************************************************
 * Variables
 *****************************************************************************/

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
    return codepoint % FONT_HASH_TABLE_LENGTH;
}

/**
 * @brief [TODO:description]
 *
 * @param font_cache [TODO:parameter]
 * @param font [TODO:parameter]
 * @return [TODO:return]
 */
bool prv_cache_valid(FontCache *font_cache, GFont font)
{
    if (font_cache->resource == &font->resource)
    {
        return true;
    }

    font_cache->offset_table_id = -1;
    font_cache->resource = &font->resource;

    return false;
}

/**
 * @brief [TODO:description]
 *
 * @param font_cache [TODO:parameter]
 * @param font [TODO:parameter]
 * @param codepoint [TODO:parameter]
 */
void prv_load_offset_table(FontCache *font_cache, GFont font, uint16_t codepoint)
{
    uint8_t hash = prv_hash_function(codepoint);

    uint32_t hash_entry_offset = (hash * sizeof(FontHashTableEntry)) + sizeof(FontMetaData);

    FontHashTableEntry hash_entry = {0};
    resource_storage_read_bytes(&font->resource.resource_entry, hash_entry_offset, &hash_entry,
                                sizeof(FontHashTableEntry));

    if (font_cache->offset_table_id == hash_entry.offset && font_cache->offset_table_length == hash_entry.count)
    {
        return;
    }

    font_cache->offset_table_id = hash_entry.offset;
    font_cache->offset_table_length = hash_entry.count;

    uint32_t entry_table_offset = OFFSET_TABLE_START_OFFSET + (hash_entry.offset * sizeof(OffsetTableEntry));
    uint32_t entry_table_size_bytes = (hash_entry.count * sizeof(OffsetTableEntry));

    resource_storage_read_bytes(&font->resource.resource_entry, entry_table_offset, font_cache->offset_table_cache,
                                entry_table_size_bytes);
}

/**
 * @brief [TODO:description]
 *
 * @param font_cache [TODO:parameter]
 * @param font [TODO:parameter]
 * @param offset_entry [TODO:parameter]
 */
void prv_load_glyph_data(FontCache *font_cache, GFont font, OffsetTableEntry *offset_entry)
{
    uint32_t offset = OFFSET_TABLE_START_OFFSET + (offset_entry->offset_words * 4);

    resource_storage_read_bytes(&font->resource.resource_entry, offset, &font_cache->last_glyph.header,
                                sizeof(GlyphHeaderData));
    resource_storage_read_bytes(&font->resource.resource_entry, offset + sizeof(GlyphHeaderData),
                                &font_cache->glyph_data_cache, 256);

    font_cache->last_glyph.data = font_cache->glyph_data_cache;

    LOG_DBG("Glyph data for codepoint 0x%04X", offset_entry->codepoint);
    LOG_DBG("Width: %u", font_cache->last_glyph.header.width);
    LOG_DBG("Height: %u", font_cache->last_glyph.header.height);
    LOG_DBG("Offset Left: %d", font_cache->last_glyph.header.offset_left);
    LOG_DBG("Offset Top: %d", font_cache->last_glyph.header.offset_top);
    LOG_DBG("Horizontal Adv.: %u", font_cache->last_glyph.header.horizontal_advance);
}

/**
 * @brief [TODO:description]
 *
 * @param font_cache [TODO:parameter]
 * @param font [TODO:parameter]
 * @param codepoint [TODO:parameter]
 * @return [TODO:return]
 */
const GlyphData *prv_get_glyph_from_cache(FontCache *font_cache, GFont font, uint16_t codepoint)
{
    prv_cache_valid(font_cache, font);

    prv_load_offset_table(font_cache, font, codepoint);

    for (size_t i = 0; i < font_cache->offset_table_length; i++)
    {
        OffsetTableEntry *entry = &font_cache->offset_table_cache[i];

        if (entry->codepoint == codepoint)
        {
            prv_load_glyph_data(font_cache, font, entry);

            return &font_cache->last_glyph;
        }
    }

    return NULL;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

bool text_resources_init_font(FontInfo *font_info, uint32_t app_num, uint32_t resource_id)
{
    bool ret = resource_storage_get_resource(app_num, resource_id, &font_info->resource.resource_entry);

    if (ret == false)
    {
        return false;
    }

    size_t read_bytes = resource_storage_read_bytes(&font_info->resource.resource_entry, 0,
                                                    &font_info->resource.meta_data, sizeof(FontMetaData));

    if (read_bytes != sizeof(FontMetaData))
    {
        return false;
    }

    font_info->loaded = true;

    return true;
}

const GlyphData *text_resources_get_glyph(FontCache *font_cache, GFont font, uint16_t codepoint)
{
    return prv_get_glyph_from_cache(font_cache, font, codepoint);
}
