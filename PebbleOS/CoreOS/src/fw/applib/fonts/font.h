/*
 * Copyright (C) Ovyl
 */

/**
 * @file font.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef font_h
#define font_h

#include <stdbool.h>
#include <stdint.h>

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
 * @typedef FontMetaData
 * @brief Definition of Font Meta Data
 *
 */
typedef struct __attribute__((packed)) FontMetaData
{
    uint8_t version;
    uint8_t max_height;
    uint16_t num_glyphs;
    uint16_t wildcard_codepoint;
    uint8_t hash_table_size;
    uint8_t codepoint_bytes;
} FontMetaData;

/**
 * @typedef FontHashTableEntry
 * @brief Definition of Hash Table entry
 *
 */
typedef struct __attribute__((packed)) FontHashTableEntry
{
    uint8_t hash;
    uint8_t count;
    uint16_t offset;
} FontHashTableEntry;

/**
 * @typedef OffsetTableEntry
 * @brief [TODO:description]
 *
 */
typedef struct __attribute__((packed)) OffsetTableEntry
{
    uint16_t codepoint;
    uint16_t offset_words;
} OffsetTableEntry;

struct FontInfo;

/**
 * @brief
 */
typedef struct FontInfo *GFont;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

GFont font_load_system_font(uint32_t font_id);

#ifdef __cplusplus
}
#endif
#endif /* font_h */
