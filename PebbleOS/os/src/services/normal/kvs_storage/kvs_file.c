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
#include <stdbool.h>
#include <stdint.h>

#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include <zephyr/sys/crc.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(kvs_file);

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
} __attribute__((__packed__)) KVS_Record_Header_t;

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

/**
 * @brief [TODO:description]
 *
 * @param kvs_file [TODO:parameter]
 * @return [TODO:return]
 * @retval -EBADMSG Header is invalid
 * @retval Negative Other error, see error code value
 * @retval 0 Header valid
 */
int prv_validate_file_header(KVS_File_t *kvs_file) {

  KVS_File_Header_t header = {0};

  int ret = pfs_seek(&kvs_file->file, 0, FS_SEEK_SET);

  if (ret < 0) {
    LOG_ERR("Failed to return to start of file: %d", ret);
    return ret;
  }

  ssize_t read_bytes = pfs_read(&kvs_file->file, &header, sizeof(header));

  if (read_bytes < 0) {
    LOG_ERR("Failed to read KVS file header: %d", read_bytes);
    return read_bytes;
  }

  if (read_bytes != sizeof(header)) {
    LOG_ERR("Corrupt or incorrect KVS file header.");
    return -EBADMSG;
  }

  if (memcmp(&header.magic, KVS_FILE_FILE_HEADER_MAGIC,
             sizeof(KVS_FILE_FILE_HEADER_MAGIC)) != 0) {
    LOG_ERR("Incorrect KVS header magic value.");
    return -EBADMSG;
  }

  kvs_file->max_file_size_bytes = header.max_file_size_bytes;
  kvs_file->max_used_space_bytes = header.max_used_space_bytes;

  return 0;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int kvs_file_exists(const char *filename) {
  if (filename == NULL) {
    return -EINVAL;
  }

  pfs_file_t file = {0};

  int ret = pfs_open(&file, filename, FS_O_READ);

  if (ret < 0) {
    LOG_ERR("Failed to open KVS file: %d", ret);
    return ret;
  }

  pfs_close(&file);

  return 1;
}

int kvs_file_open(KVS_File_t *kvs_file, const char *filename) {
  if (kvs_file == NULL || filename == NULL) {
    return -EINVAL;
  }

  int ret = kvs_file_exists(filename);

  if (ret == 0) {
    return -ENOENT;
  }

  if (ret < 0) {
    return ret;
  }

  ret = pfs_open(&kvs_file->file, filename, FS_O_RDWR | FS_O_APPEND);

  if (ret < 0) {
    LOG_ERR("Failed to open KVS File %s: %d", filename, ret);
    return ret;
  }

  ret = prv_validate_file_header(kvs_file);

  if (ret < 0) {
    LOG_ERR("Failed to validate KVS file header: %d", ret);

    pfs_close(&kvs_file->file);
    return ret;
  }

  return 0;
}

int kvs_file_create(KVS_File_t *kvs_file, const char *filename,
                    size_t max_used_space_bytes) {
  if (kvs_file == NULL || filename == NULL ||
      max_used_space_bytes < KVS_FILE_MINIMUM_USED_SPACE_BYTES) {
    return -EINVAL;
  }

  int ret = kvs_file_exists(filename);

  if (ret == 1) {
    LOG_WRN("KVS file (%s) already exists.", filename);
    return -EALREADY;
  }

  ret = pfs_open(&kvs_file->file, filename,
                 FS_O_RDWR | FS_O_CREATE | FS_O_APPEND);

  if (ret < 0) {
    LOG_ERR("Failed to create KVS file %s: %d", filename, ret);
    return ret;
  }

  KVS_File_Header_t header = {0};
  memcpy(&header, KVS_FILE_FILE_HEADER_MAGIC,
         sizeof(KVS_FILE_FILE_HEADER_MAGIC));

  header.max_used_space_bytes = max_used_space_bytes;
  header.max_file_size_bytes = max_used_space_bytes;
  header.max_file_size_bytes += (max_used_space_bytes * 100) / KVS_PADDING_PCT;

  ret = pfs_write(&kvs_file->file, &header, sizeof(header));

  if (ret < 0) {
    LOG_ERR("Failed to write header to KVS File: %d", ret);
    return ret;
  }

  if (ret != sizeof(header)) {
    LOG_ERR("Failed to write complete header to KVS File.");
    return -EIO;
  }

  ret = pfs_sync(&kvs_file->file);

  if (ret < 0) {
    LOG_ERR("Failed to flush KVS file: %d", ret);
    return ret;
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

/*****************************************************************************
 * Shell Functions
 *****************************************************************************/

#ifdef CONFIG_SHELL

static int prv_shell_open_file(const struct shell *sh, size_t argc,
                               char **argv) {
  static KVS_File_t file = {0};

  int ret = kvs_file_open(&file, argv[1]);

  if (ret < 0) {
    shell_error(sh, "Failed to open KVS file: %d", ret);
    return ret;
  }

  return 0;
}

static int prv_shell_create_file(const struct shell *sh, size_t argc,
                                 char **argv) {
  static KVS_File_t file = {0};

  int ret = kvs_file_create(&file, argv[1], atoi(argv[2]));

  if (ret < 0) {
    shell_print(sh, "Failed to create KVS file: %d", ret);
    return ret;
  }

  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_kvs,
                               SHELL_CMD_ARG(open, NULL, "Open KVS file.",
                                             prv_shell_open_file, 2, 0),
                               SHELL_CMD_ARG(create, NULL, "Close open file.",
                                             prv_shell_create_file, 3, 0),
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(kvs, &sub_kvs, "Key/Value Storage", NULL);

#endif
