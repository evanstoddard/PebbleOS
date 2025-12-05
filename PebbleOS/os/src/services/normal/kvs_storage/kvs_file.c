/*
 * Copyright (C) Ovyl
 */

/**
 * @file kvs_file.c
 * @author Evan Stoddard
 * @brief
 */

#include "kvs_file.h"

#include <errno.h>

#include <zephyr/logging/log.h>

#include <zephyr/sys/crc.h>

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

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Variables
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
  uint32_t key_size_bytes;
  uint32_t value_size_bytes;
  uint32_t flags;
  uint8_t key_hash;
  uint8_t unused[3];
} __attribute__((__struct__)) KVS_RECORD_Header_t;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param key [TODO:parameter]
 * @param key_len_bytes [TODO:parameter]
 * @return [TODO:return]
 */
uint8_t prv_hash_for_key(const void *key, const size_t key_len_bytes) {
  if (key == NULL || key_len_bytes == 0) {
    return 0;
  }

  return crc8_rohc(0, key, key_len_bytes);
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int kvs_file_exists(const char *filename) {
  if (filename == NULL) {
    return -EINVAL;
  }

  return 0;
}

int kvs_file_open(KVS_File_t *kvs_file, const char *filename) {
  if (kvs_file == NULL || filename == NULL) {
    return -EINVAL;
  }

  return 0;
}

int kvs_file_create(KVS_File_t *kvs_file, const char *filename,
                    size_t max_used_space_bytes) {
  if (kvs_file == NULL || filename == NULL ||
      max_used_space_bytes < KVS_FILE_MINIMUM_USED_SPACE_BYTES) {
    return -EINVAL;
  }

  return 0;
}

int kvs_file_set_pair(KVS_File_t *kvs_file, const void *key,
                      const size_t key_len_bytes, const void *value,
                      const size_t value_length_bytes) {
  if (kvs_file == NULL || key == NULL || value == NULL || key_len_bytes == 0 ||
      value_length_bytes == 0) {
    return -EINVAL;
  }

  return 0;
}

int kvs_file_get_value_len(KVS_File_t *kvs_file, const void *key,
                           const size_t key_len_bytes,
                           size_t *value_len_bytes) {
  if (kvs_file == NULL || key == NULL || value_len_bytes == NULL ||
      key_len_bytes == 0) {
    return -EINVAL;
  }

  return 0;
}

int kvs_file_get_pair(KVS_File_t *kvs_file, const void *key,
                      const size_t key_len_bytes, void *dst, size_t dst_buf) {
  if (kvs_file == NULL || key == NULL || dst == NULL || key_len_bytes == 0 ||
      dst_buf == 0) {
    return -EINVAL;
  }

  return 0;
}

int kvs_file_delete_pair(KVS_File_t *kvs_file, const void *key,
                         const size_t key_len_bytes) {
  if (kvs_file == NULL || key == NULL || key_len_bytes == 0) {
    return -EINVAL;
  }

  return 0;
}
