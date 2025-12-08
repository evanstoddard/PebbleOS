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
#include <stdlib.h>

#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include "kvs_iterator.h"
#include "services/normal/kvs_storage/kvs_types.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(kvs_file);

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param kvs_file [TODO:parameter]
 */
static void prv_update_stats(KVS_File_t *kvs_file) {}

/**
 * @brief [TODO:description]
 *
 * @param kvs_file [TODO:parameter]
 * @return [TODO:return]
 */
static int prv_validate_kvs_file(KVS_File_t *kvs_file) {
  kvs_file->max_file_size_bytes =
      kvs_file->iterator.file_header.max_file_size_bytes;
  kvs_file->max_used_space_bytes =
      kvs_file->iterator.file_header.max_used_space_bytes;

  int ret = kvs_iterator_reset(&kvs_file->iterator);
  if (ret < 0) {
    LOG_ERR("Failed to reset iterator: %d", ret);

    return -EIO;
  }

  while (true) {
    ret = kvs_iterator_next_record(&kvs_file->iterator);
    if (ret < 0) {
      LOG_WRN("Invalid record: %d", ret);
      return ret;
    }

    if (ret == 0) {
      LOG_INF("Found EOF Record.");
      return ret;
    }

    prv_update_stats(kvs_file);
  }

  return 0;
}

/**
 * @brief [TODO:description]
 *
 * @param file [TODO:parameter]
 * @return [TODO:return]
 */
static int prv_repair_file(KVS_File_t *file) {
  // TODO: All of this...
  
  return -ENOTSUP;
}

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param record_offset [TODO:parameter]
 * @param record_header [TODO:parameter]
 * @param ctx [TODO:parameter]
 * @return [TODO:return]
 */
static int prv_mark_record_overwrite_pending(KVS_Iterator_t *iterator, off_t record_offset,
                  KVS_Record_Header_t *record_header, void *ctx) {
 return kvs_iterator_clear_flags(iterator, record_header, record_offset, KVS_RECORD_FLAG_OVERWRITE_PENDING);
}

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param record_offset [TODO:parameter]
 * @param record_header [TODO:parameter]
 * @param ctx [TODO:parameter]
 * @return [TODO:return]
 */
static int prv_mark_record_overwrite_complete(KVS_Iterator_t *iterator, off_t record_offset,
                  KVS_Record_Header_t *record_header, void *ctx) {
  return kvs_iterator_clear_flags(iterator, record_header, record_offset, KVS_RECORD_FLAG_OVERWRITE_COMPLETE);
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

  kvs_iterator_init(&kvs_file->iterator, &kvs_file->file);

  ret = kvs_iterator_fetch_file_header(&kvs_file->iterator);
  if (ret < 0) {
    LOG_ERR("Failed to validate KVS file header: %d", ret);

    pfs_close(&kvs_file->file);
    return ret;
  }

  ret = prv_validate_kvs_file(kvs_file);
  if (ret == -EIO) {
    LOG_ERR("Failed to validate KVS file.");
    return -EIO;
  }

  if (ret == -EBADMSG) {
    LOG_WRN("Attempting to repair KVS file.");

    ret = prv_repair_file(kvs_file);

    if (ret < 0) {
      LOG_ERR("Failed to repair KVS file: %d", ret);

      return -EBADMSG;
    }
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

  KVS_Record_Header_t record_header = {0};
  memset(&record_header, 0xFF, sizeof(KVS_Record_Header_t));
  memcpy(record_header.magic, KVS_FILE_RECORD_HEADER_MAGIC,
         sizeof(KVS_FILE_RECORD_HEADER_MAGIC));

  ret = pfs_write(&kvs_file->file, &record_header, sizeof(KVS_Record_Header_t));

  if (ret != sizeof(record_header)) {
    LOG_ERR("Unexected return while writing KVS EOF Record: %d", ret);
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
  
  KVS_Record_Filter_t filter = {
    .key = key,
    .key_len = key_len_bytes,
    .flags = 0x0,
    .exact_flag_match = true
  };
  
  KVS_Record_Foreach_Callback_t callback = {
    .callback = prv_mark_record_overwrite_pending,
    .ctx = kvs_file
  };

  int ret = kvs_iterator_filtered_foreach_record(&kvs_file->iterator, &filter, &callback);
  
  if (ret < 0 && ret != -ENOENT) {
    LOG_ERR("Failed to iterate through records: %d", ret);
    return ret;
  }
 
  KVS_Record_Header_t record_header = {0};
  kvs_init_record_header(&record_header);

  // Don't clear MSB of key size as clearing that indicates the record is valid.  Clearing this at the end ensures that the file won't be corrupt if writing only partially completes.
  record_header.key_size_bytes = KEY_RECORD_KEY_SIZE_EOF_MASK | (uint8_t)key_len_bytes;
  record_header.key_hash = kvs_hash_for_key(key, key_len_bytes);
  record_header.value_size_bytes = value_length_bytes;

  ret = pfs_seek(&kvs_file->file, -((off_t)sizeof(KVS_Record_Header_t)), FS_SEEK_END);
  if (ret < 0) {
    LOG_ERR("Failed to move pointer to EOF record: %d", ret);
    return ret;
  }
  
  off_t record_offset = pfs_tell(&kvs_file->file);
  if (record_offset < 0) {
    LOG_ERR("Failed to find offset of EOF record: %ld", record_offset);
    return record_offset;
  }
  
  ret = pfs_write(&kvs_file->file, &record_header, sizeof(record_header));
  if (ret < 0) {
    LOG_ERR("Failed to write partial header: %d", ret);
    return ret;
  }

  ret = pfs_write(&kvs_file->file, key, key_len_bytes);
  if (ret < 0) {
    LOG_ERR("Failed to write key: %d", ret);
    return ret;
  }
  
  ret = pfs_write(&kvs_file->file, value, value_length_bytes);
  if (ret < 0) {
    LOG_ERR("Failed to write value: %d", ret);
    return ret;
  }
  
  callback.callback = prv_mark_record_overwrite_complete;
  filter.flags |= KVS_OVERWRITE_IN_PROGRESS_MASK;
  ret = kvs_iterator_filtered_foreach_record(&kvs_file->iterator, &filter, &callback);
  
  if (ret < 0 && ret != -ENOENT) {
    LOG_ERR("Failed to mark overwrite complete: %d", ret);
    return ret;
  }
  
  record_header.key_size_bytes &= ~(KEY_RECORD_KEY_SIZE_EOF_MASK);

  ret = pfs_seek(&kvs_file->file, record_offset, FS_SEEK_SET);
  if (ret < 0) {
    LOG_ERR("Failed to move to new record header: %d", ret);
    return ret;
  }

  ret = pfs_write(&kvs_file->file, &record_header, sizeof(record_header));
  if (ret < 0) {
    LOG_ERR("Failed to mark new record as valid: %d", ret);
    return ret;
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
