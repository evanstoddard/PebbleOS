/*
 * Copyright (C) Ovyl
 */

/**
 * @file main.c
 * @author Evan Stoddard
 * @brief
 */

#include <stdio.h>

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "font.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
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

static struct
{
    int font_file;
    void *mapped_font;
    FontMetaData font_md;
    FontHashTableEntry font_hash_table[255];
    SDL_Window *window;
    SDL_Renderer *renderer;
    struct
    {
        uint32_t x;
        uint32_t y;
    } cursor;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param msg [TODO:parameter]
 * @param err [TODO:parameter]
 */
void die(char *msg, int err)
{
    printf("Error: %s\r\n", msg);

    exit(err);
}

/**
 * @brief [TODO:description]
 *
 * @param path [TODO:parameter]
 * @return [TODO:return]
 */
bool prv_load_font_file(char *path)
{
    prv_inst.font_file = open(path, O_RDONLY, "rb");

    if (prv_inst.font_file <= 0)
    {
        printf("Failed to open file.\r\n");
        return false;
    }

    struct stat fs = {0};
    int ret = fstat(prv_inst.font_file, &fs);

    if (ret != 0)
    {
        printf("Failed to stat file: %d\r\n", ret);
        return false;
    }

    prv_inst.mapped_font = mmap(0, fs.st_size, PROT_READ, MAP_PRIVATE, prv_inst.font_file, 0);

    if (prv_inst.mapped_font == MAP_FAILED)
    {
        printf("Failed to map file.\r\n");
        return false;
    }

    return true;
}

/**
 * @brief [TODO:description]
 *
 * @param dst [TODO:parameter]
 * @param offset [TODO:parameter]
 * @param length [TODO:parameter]
 */
void prv_read(void *dst, size_t offset, size_t length)
{
    uintptr_t ptr = (uintptr_t)prv_inst.mapped_font + offset;

    memcpy(dst, (const void *)ptr, length);
}

/**
 * @brief [TODO:description]
 *
 * @param md [TODO:parameter]
 */
void prv_load_font_meta_data(FontMetaData *md)
{
    prv_read(md, 0, sizeof(FontMetaData));
}

/**
 * @brief [TODO:description]
 */
void prv_log_font_metadata(FontMetaData *meta_data)
{
    printf("Font Metadata:\r\n");
    printf("\tVersion: %u\r\n", meta_data->version);
    printf("\tMax Height: %u\r\n", meta_data->max_height);
    printf("\tNum Glyphs: %u\r\n", meta_data->num_glyphs);
    printf("\tWildcard Codepoint: %u\r\n", meta_data->wildcard_codepoint);
    printf("\tHash Table Size: %u\r\n", meta_data->hash_table_size);
    printf("\tCodepoint Bytes: %u\r\n", meta_data->codepoint_bytes);
    printf("\r\n");
}

/**
 * @brief [TODO:description]
 */
void prv_load_font_hash_table(void)
{
    size_t offset = sizeof(FontMetaData);
    prv_read(prv_inst.font_hash_table, offset, sizeof(FontHashTableEntry) * 255);
}

/**
 * @brief [TODO:description]
 */
void prv_log_hash_table(void)
{
    for (size_t i = 0; i < 255; i++)
    {
        FontHashTableEntry *entry = &prv_inst.font_hash_table[i];
        printf("Hash Table Entry 0x%02X:\r\n", i);
        printf("\tCount: %u\r\n", entry->count);
        printf("\tOffset Entry Offset: 0x%08X\r\n", entry->offset);
        printf("\r\n");
    }
}

/**
 * @brief [TODO:description]
 *
 * @param entry [TODO:parameter]
 * @param idx [TODO:parameter]
 */
void prv_get_offset_entry_for_index(OffsetTableEntry *entry, size_t idx)
{
    size_t offset = sizeof(FontMetaData) + (sizeof(FontHashTableEntry) * 255);
    offset += (idx * sizeof(OffsetTableEntry));

    prv_read(entry, offset, sizeof(OffsetTableEntry));
}

/**
 * @brief [TODO:description]
 *
 * @param idx [TODO:parameter]
 */
void prv_log_offset_entry(OffsetTableEntry *entry)
{
    printf("Codepoint 0x%08X:\r\n", entry->codepoint);
    printf("\tOffset Words: 0x%08X\r\n", entry->offset_words);
    printf("\r\n");
}

/**
 * @brief [TODO:description]
 */
void prv_create_sdl_window(void)
{
    int err = SDL_Init(SDL_INIT_VIDEO);

    if (err != 0)
    {
        die("Failed to create SDL Window.", err);
    }

    err = SDL_CreateWindowAndRenderer(640, 480, 0, &prv_inst.window, &prv_inst.renderer);
    if (err != 0)
    {
        die("Failed to create window & renderer.", err);
    }
}

/**
 * @brief [TODO:description]
 *
 * @param x [TODO:parameter]
 * @param y [TODO:parameter]
 * @param on [TODO:parameter]
 */
void prv_draw_point(uint32_t x, uint32_t y, bool on)
{
    if (on)
    {
        SDL_SetRenderDrawColor(prv_inst.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }
    else
    {
        SDL_SetRenderDrawColor(prv_inst.renderer, 0x0, 0x0, 0x0, 0x0);
    }

    SDL_RenderDrawPoint(prv_inst.renderer, x, y);
}

/**
 * @brief [TODO:description]
 *
 * @param idx [TODO:parameter]
 */
void prv_draw_glyph_at_idx(size_t idx)
{
    OffsetTableEntry entry = {0};
    prv_get_offset_entry_for_index(&entry, idx);

    size_t offset = sizeof(FontMetaData);
    offset += (sizeof(FontHashTableEntry) * 255);
    offset += entry.offset_words * 4;

    GlyphData glyph_data = {0};
    prv_read(&glyph_data, offset, sizeof(GlyphHeaderData));

    uint8_t glyph_buf[256] = {0};
    prv_read(glyph_buf, offset + sizeof(GlyphHeaderData), 256);

    uint8_t bytes_per_row = glyph_data.header.width >> 3;

    printf("\tGlyph Data: ");
    printf("\t\tWidth: %u\r\n", glyph_data.header.width);
    printf("\t\tHeight: %u\r\n", glyph_data.header.height);
    printf("\t\tOffset Left: %d\r\n", glyph_data.header.offset_left);
    printf("\t\tOffset Right: %d\r\n", glyph_data.header.offset_top);
    printf("\t\tHorizontal Advance: %u\r\n", glyph_data.header.horizontal_advance);

    if (glyph_data.header.width & 0x7)
    {
        bytes_per_row++;
    }

    if (bytes_per_row % 2)
    {
        bytes_per_row++;
    }

    if ((prv_inst.cursor.x + glyph_data.header.horizontal_advance + glyph_data.header.width) > 639)
    {
        prv_inst.cursor.x = 0;
        prv_inst.cursor.y += prv_inst.font_md.max_height;
    }
    printf("\t\tBytes Per Row: %u\r\n", bytes_per_row);
    uint32_t _x = prv_inst.cursor.x;
    uint32_t _y = prv_inst.cursor.y;

    for (uint32_t y = 0; y < glyph_data.header.height; y++)
    {
        for (uint32_t x = 0; x < glyph_data.header.width; x++)
        {
            uint32_t idx = y * bytes_per_row;
            idx += x >> 3;
            uint8_t shift = (7 - (x & 0x7));
            uint8_t mask = (1 << shift);

            if (glyph_buf[idx] & mask)
            {
                /* printf("X");  */
                prv_draw_point(_x + x, _y + y, true);
            }
            else
            {
                /* printf(" "); */
            }
        }
    }

    prv_inst.cursor.x += glyph_data.header.width + glyph_data.header.horizontal_advance;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        die("Please provide a font file.", -1);
    }

    bool ret = prv_load_font_file(argv[1]);

    if (ret == false)
    {
        die("Failed to open font.", -1);
    }

    prv_create_sdl_window();

    FontMetaData md = {0};
    prv_load_font_meta_data(&prv_inst.font_md);
    prv_log_font_metadata(&prv_inst.font_md);
    prv_load_font_hash_table();
    prv_log_hash_table();

    /* prv_draw_glyph_at_idx(1); */

    for (size_t i = 0; i < prv_inst.font_md.num_glyphs; i++)
    {
        OffsetTableEntry entry = {0};
        prv_get_offset_entry_for_index(&entry, i);
        prv_log_offset_entry(&entry);
        prv_draw_glyph_at_idx(i);
        printf("\n");
    }

    SDL_RenderPresent(prv_inst.renderer);

    SDL_Event event;
    while (1)
    {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }

    return 0;
}
