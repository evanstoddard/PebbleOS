/*
 * Copyright (C) Ovyl
 */

/**
 * @file kvs_types.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef kvs_types_h
#define kvs_types_h

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <zephyr/sys/crc.h>

#include "services/normal/filesystem/pfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/**
 * @brief Percentage of used space to add as padded bytes as a threshold to
 * begin garbage collection.  Percentage is most definitely not the best basis
 * to use for padding, but it was quick and dirty and provided some semblence of
 * scaling based on input size.
 */
#define KVS_PADDING_PCT (20U)

#define KVS_FILE_FILE_HEADER_MAGIC "KVS"

#define KVS_FILE_RECORD_HEADER_MAGIC "RCD"

#define KEY_RECORD_KEY_SIZE_EOF_MASK 0x80U

#define KVS_MAX_KEY_LEN 0x7F

#define KVS_OVERWRITE_IN_PROGRESS_MASK (0x80)

#define KVS_OVERWRITE_COMPLETE_MASK (0x40)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef KVS_File_Header_t
 * @brief [TODO:description]
 *
 */
typedef struct KVS_File_Header_t {
  char magic[sizeof(KVS_FILE_FILE_HEADER_MAGIC)];

  // NOTE: Explicitly defining size limits as 32-bit int for portablility.
  uint32_t max_used_space_bytes;
  uint32_t max_file_size_bytes;
} __attribute__((__packed__)) KVS_File_Header_t;

/**
 * @typedef KVS_Record_Header_t
 * @brief [TODO:description]
 *
 */
typedef struct KVS_Record_Header_t {
  char magic[sizeof(KVS_FILE_RECORD_HEADER_MAGIC)];

  uint8_t key_size_bytes;
  uint8_t value_size_bytes;
  uint8_t key_hash;
  uint8_t flags;
} __attribute__((__packed__)) KVS_Record_Header_t;

/**
 * @typedef KVS_Iterator_t
 * @brief [TODO:description]
 *
 */
typedef struct KVS_Iterator_t {
  pfs_file_t *file;

  KVS_File_Header_t file_header;

  KVS_Record_Header_t current_record_header;

  off_t current_header_position;

} KVS_Iterator_t;

/**
 * @typedef KVS_Record_Filter_t
 * @brief
 *
 */
typedef struct KVS_Record_Filter_t {
  const void *key;
  size_t key_len;
  uint8_t key_hash;

  uint8_t flags;
  bool exact_flag_match;
} KVS_Record_Filter_t;

/**
 * @typedef KVS_File_t
 * @brief [TODO:description]
 *
 */
typedef struct KVS_File_t {
  KVS_Iterator_t iterator;

  const char *filename;

  pfs_file_t file;

  // Limits
  size_t max_used_space_bytes;
  size_t max_file_size_bytes;

  // Usage
  size_t used_space;
  size_t dead_space_bytes;
} KVS_File_t;

/*
 * TODO: This should really be refactored so it passes a compound type which
 * contains the header, header offset, key offset, and value offset instead of
 * breaking at all out separately.  One could argue it makes the API messy.
 */

/**
 * @typedef KVS_Record_Foreach_Callback_t
 * @brief
 *
 */
typedef struct KVS_Record_Foreach_Callback_t {
  int (*callback)(KVS_Iterator_t *iterator, off_t record_offset, KVS_Record_Header_t *record_header,
                  void *ctx);
  void *ctx;
} KVS_Record_Foreach_Callback_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Static Inlined Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param record_header [TODO:parameter]
 * @return [TODO:return]
 */
static inline int kvs_init_record_header(KVS_Record_Header_t *record_header) {
  if (record_header == NULL) {
    return -EINVAL;
  }

  memset(record_header, 0xFF, sizeof(KVS_Record_Header_t));
  memcpy(record_header->magic, KVS_FILE_RECORD_HEADER_MAGIC, sizeof(KVS_FILE_RECORD_HEADER_MAGIC));

  return 0;
}

/**
 * @brief [TODO:description]
 *
 * @param key [TODO:parameter]
 * @param key_len_bytes [TODO:parameter]
 * @return [TODO:return]
 */
static inline uint8_t kvs_hash_for_key(const void *key, const size_t key_len_bytes) {
  if (key == NULL || key_len_bytes == 0) {
    return 0;
  }

  return crc8_rohc(0, key, key_len_bytes);
}

#ifdef __cplusplus
}
#endif
#endif /* kvs_types_h */
