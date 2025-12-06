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

#include <stddef.h>

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

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* kvs_types_h */
