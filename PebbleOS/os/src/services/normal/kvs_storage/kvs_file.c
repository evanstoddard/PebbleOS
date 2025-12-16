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
static int prv_mark_record_overwrite_pending(KVS_Iterator_t *iterator,
                                             off_t record_offset,
                                             KVS_Record_Header_t *record_header,
                                             void *ctx) {
  return kvs_iterator_clear_flags(iterator, record_header, record_offset,
                                  KVS_RECORD_FLAG_OVERWRITE_PENDING);
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
static int prv_mark_record_overwrite_complete(
    KVS_Iterator_t *iterator, off_t record_offset,
    KVS_Record_Header_t *record_header, void *ctx) {
  return kvs_iterator_clear_flags(iterator, record_header, record_offset,
                                  KVS_RECORD_FLAG_OVERWRITE_COMPLETE);
}

/**
 * @brief [TODO:description]
 *
 * @param kvs_file [TODO:parameter]
 * @param key [TODO:parameter]
 * @param key_len [TODO:parameter]
 * @return [TODO:return]
 */
static inline int prv_mark_records_overwrite_pending(KVS_File_t *kvs_file,
                                                     const void *key,
                                                     const size_t key_len) {
  KVS_Record_Filter_t filter = {
      .key = key, .key_len = key_len, .flags = 0x0, .exact_flag_match = true};

  KVS_Record_Foreach_Callback_t callback = {
      .callback = prv_mark_record_overwrite_pending, .ctx = kvs_file};

  int ret = kvs_iterator_filtered_foreach_record(&kvs_file->iterator, &filter,
                                                 &callback);

  if (ret < 0 && ret != -ENOENT) {
    LOG_ERR("Failed to iterate through records: %d", ret);
    return ret;
  }

  return 0;
}

/**
 * @brief [TODO:description]
 *
 * @param kvs_file [TODO:parameter]
 * @param key [TODO:parameter]
 * @param key_len [TODO:parameter]
 * @return [TODO:return]
 */
static inline int prv_mark_records_overwrite_complete(KVS_File_t *kvs_file,
                                                      const void *key,
                                                      const size_t key_len) {
  KVS_Record_Filter_t filter = {
      .key = key, .key_len = key_len, .flags = 0x0, .exact_flag_match = true};
  filter.flags |= KVS_OVERWRITE_IN_PROGRESS_MASK;

  KVS_Record_Foreach_Callback_t callback = {
      .callback = prv_mark_record_overwrite_complete, .ctx = kvs_file};

  int ret = kvs_iterator_filtered_foreach_record(&kvs_file->iterator, &filter,
                                                 &callback);

  if (ret < 0 && ret != -ENOENT) {
    LOG_ERR("Failed to iterate through records: %d", ret);
    return ret;
  }

  return 0;
}

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @return [TODO:return]
 */
static int prv_write_eof_record_header(KVS_Iterator_t *iterator) {
  KVS_Record_Header_t header = {0};
  kvs_init_record_header(&header);

  // Move to end of file
  int ret = pfs_seek(iterator->file, 0, FS_SEEK_END);
  if (ret < 0) {
    LOG_ERR("Failed to move pointer to EOF: %d", ret);
    return ret;
  }

  off_t eof_offset = pfs_tell(iterator->file);
  if (eof_offset < 0) {
    LOG_ERR("Failed to determine EOF offset: %ld", eof_offset);
    return eof_offset;
  }

  ret = pfs_truncate(iterator->file, eof_offset + sizeof(header));
  if (ret < 0) {
    LOG_ERR("Failed to extend KVS file: %d", ret);
    return ret;
  }

  ret = pfs_write(iterator->file, &header, sizeof(header));

  return ret;
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

  ret = pfs_open(&kvs_file->file, filename, FS_O_RDWR);

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

  ret = pfs_open(&kvs_file->file, filename, FS_O_RDWR | FS_O_CREATE);

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

  ret = pfs_truncate(&kvs_file->file,
                     sizeof(KVS_File_Header_t) + sizeof(KVS_Record_Header_t));
  if (ret < 0) {
    LOG_ERR("Failed to extend new KVS file: %d", ret);
    return ret;
  }

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

  // Next couple lines are somewhat redundent, be helps ensure KVS file was
  // created successfull and prevents me from duplicating Initialization
  pfs_close(&kvs_file->file);

  return kvs_file_open(kvs_file, filename);
}

int kvs_file_set_pair(KVS_File_t *kvs_file, const void *key,
                      const size_t key_len_bytes, const void *value,
                      const size_t value_length_bytes) {
  if (kvs_file == NULL || key == NULL || value == NULL || key_len_bytes == 0 ||
      value_length_bytes == 0) {
    return -EINVAL;
  }

  KVS_Record_Header_t record_header = {0};
  kvs_init_record_header(&record_header);

  // Don't clear MSB of key size as clearing that indicates the record is valid.
  // Clearing this at the end ensures that the file won't be corrupt if writing
  // only partially completes.
  record_header.key_size_bytes =
      KEY_RECORD_KEY_SIZE_EOF_MASK | (uint8_t)key_len_bytes;
  record_header.key_hash = kvs_hash_for_key(key, key_len_bytes);
  record_header.value_size_bytes = value_length_bytes;

  int ret = prv_mark_records_overwrite_pending(kvs_file, key, key_len_bytes);
  if (ret < 0) {
    LOG_ERR("Failed to mark previous records as overwrite pending: %d", ret);
    return ret;
  }

  ret = pfs_seek(&kvs_file->file, 0, FS_SEEK_END);
  if (ret < 0) {
    LOG_ERR("Failed to move pointer to EOF: %d", ret);
    return ret;
  }

  // Move file pointer to EOF record
  off_t record_offset = pfs_tell(&kvs_file->file);
  record_offset -= sizeof(KVS_Record_Header_t);
  LOG_INF("Pointer: %ld", record_offset);

  if (record_offset < 0) {
    LOG_ERR("Failed to find offset of EOF record: %ld", record_offset);
    return record_offset;
  }

  // Extend file to include new key and value
  ret = pfs_truncate(&kvs_file->file,
                     record_offset + key_len_bytes + value_length_bytes);
  if (ret < 0) {
    LOG_ERR("Failed to extend KVS file: %d", ret);
    return ret;
  }

  ret = pfs_seek(&kvs_file->file, record_offset, FS_SEEK_SET);
  if (ret < 0) {
    LOG_ERR("Failed to move to record header: %d", ret);
    return ret;
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

  ret = prv_mark_records_overwrite_complete(kvs_file, key, key_len_bytes);
  if (ret < 0) {
    LOG_ERR("Failed to mark previous records as overwritten: %d", ret);
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
    LOG_ERR("Failed to confirm latest record: %d", ret);
    return ret;
  }

  ret = prv_write_eof_record_header(&kvs_file->iterator);
  if (ret < 0) {
    LOG_ERR("Failed to write EOF record header: %d", ret);
    return ret;
  }

  ret = pfs_sync(&kvs_file->file);

  return ret;
}

int kvs_file_get_value_len(KVS_File_t *kvs_file, const void *key,
                           const size_t key_len_bytes,
                           size_t *value_len_bytes) {
  if (kvs_file == NULL || key == NULL || value_len_bytes == NULL ||
      key_len_bytes == 0) {
    return -EINVAL;
  }

  KVS_Record_Filter_t filter = {.key = key,
                                .key_len = key_len_bytes,
                                .flags = 0x0,
                                .exact_flag_match = true};

  KVS_Record_Header_t header = {0};
  off_t header_offset = 0;

  int ret = kvs_iterator_first_occurence(&kvs_file->iterator, &filter,
                                         &header_offset, &header);

  if (ret < 0) {
    return ret;
  }

  *value_len_bytes = header.value_size_bytes;

  return 0;
}

int kvs_file_get_pair(KVS_File_t *kvs_file, const void *key,
                      const size_t key_len_bytes, void *dst, size_t dst_buf) {
  if (kvs_file == NULL || key == NULL || dst == NULL || key_len_bytes == 0 ||
      dst_buf == 0) {
    return -EINVAL;
  }

  KVS_Record_Filter_t filter = {.key = key,
                                .key_len = key_len_bytes,
                                .flags = 0x0,
                                .exact_flag_match = true};

  KVS_Record_Header_t header = {0};
  off_t header_offset = 0;

  int ret = kvs_iterator_first_occurence(&kvs_file->iterator, &filter,
                                         &header_offset, &header);

  if (ret < 0) {
    return ret;
  }

  if (dst_buf < header.value_size_bytes) {
    return -ENOMEM;
  }

  off_t value_offset =
      header_offset + sizeof(KVS_Record_Header_t) + header.key_size_bytes;
  ret = pfs_seek(&kvs_file->file, value_offset, FS_SEEK_SET);

  if (ret < 0) {
    LOG_ERR("Failed to move to record value offset: %d", ret);
    return ret;
  }

  ssize_t bytes = pfs_read(&kvs_file->file, dst, header.value_size_bytes);
  if (bytes < 0) {
    LOG_ERR("Failed to read record value: %d", bytes);
    return bytes;
  }

  if (bytes != header.value_size_bytes) {
    LOG_ERR("Unable to read complete value.  Potential record corruption.");
    return -EIO;
  }

  return 0;
}

int kvs_file_delete_pair(KVS_File_t *kvs_file, const void *key,
                         const size_t key_len_bytes) {
  if (kvs_file == NULL || key == NULL || key_len_bytes == 0) {
    return -EINVAL;
  }

  KVS_Record_Filter_t filter = {.key = key,
                                .key_len = key_len_bytes,
                                .flags = 0x0,
                                .exact_flag_match = false};

  KVS_Record_Foreach_Callback_t callback = {
      .callback = prv_mark_record_overwrite_pending, .ctx = kvs_file};

  int ret = kvs_iterator_filtered_foreach_record(&kvs_file->iterator, &filter,
                                                 &callback);

  if (ret < 0 && ret != -ENOENT) {
    return ret;
  }

  callback.callback = prv_mark_record_overwrite_complete;

  ret = kvs_iterator_filtered_foreach_record(&kvs_file->iterator, &filter,
                                             &callback);

  if (ret < 0) {
    return ret;
  }

  ret = pfs_sync(&kvs_file->file);

  return ret;
}

/*****************************************************************************
 * Shell Functions
 *****************************************************************************/

#ifdef CONFIG_SHELL

static KVS_File_t prv_shell_file = {0};

static bool prv_shell_file_open = false;

static int prv_shell_open_file(const struct shell *sh, size_t argc,
                               char **argv) {
  int ret = kvs_file_open(&prv_shell_file, argv[1]);

  if (ret < 0) {
    shell_error(sh, "Failed to open KVS file: %d", ret);
    return ret;
  }

  prv_shell_file_open = true;

  return 0;
}

static int prv_shell_create_file(const struct shell *sh, size_t argc,
                                 char **argv) {
  int ret = kvs_file_create(&prv_shell_file, argv[1], atoi(argv[2]));

  if (ret < 0) {
    shell_print(sh, "Failed to create KVS file: %d", ret);
    return ret;
  }

  prv_shell_file_open = true;

  return 0;
}

static int prv_shell_fetch_value(const struct shell *sh, size_t argc,
                                 char **argv) {

  if (prv_shell_file_open == false) {
    shell_warn(sh, "Open KVS file first.");
    return -EIO;
  }

  char value_buf[256] = {0};

  size_t value_size = 0;

  int ret = kvs_file_get_value_len(&prv_shell_file, argv[1],
                                   strlen(argv[1]) + 1, &value_size);
  if (ret == -ENOENT) {
    shell_warn(sh, "Key not found.");
    return -ENOENT;
  }

  if (ret < 0) {
    shell_error(sh, "Error fetching record length: %d", ret);
    return ret;
  }

  ret = kvs_file_get_pair(&prv_shell_file, argv[1], strlen(argv[1]) + 1,
                          value_buf, sizeof(value_buf));
  if (ret < 0) {
    shell_error(sh, "Failed to get record value: %d", ret);
    return ret;
  }

  LOG_HEXDUMP_INF(value_buf, value_size, "Record value:");

  return 0;
}

static int prv_shell_delete_value(const struct shell *sh, size_t argc,
                                  char **argv) {

  if (prv_shell_file_open == false) {
    shell_warn(sh, "Open KVS file first.");
    return -EIO;
  }

  int ret = kvs_file_delete_pair(&prv_shell_file, argv[1], strlen(argv[1]) + 1);

  if (ret == -ENOENT) {
    shell_warn(sh, "Key does not exist.");
    return -ENOENT;
  }

  if (ret < 0) {
    shell_error(sh, "Failed to delete record: %d", ret);
    return ret;
  }

  return 0;
}

static int prv_shell_set_value(const struct shell *sh, size_t argc,
                               char **argv) {
  if (prv_shell_file_open == false) {
    shell_warn(sh, "Open KVS file first.");
    return -EIO;
  }

  int ret = kvs_file_set_pair(&prv_shell_file, argv[1], strlen(argv[1]) + 1,
                              argv[2], strlen(argv[2]) + 1);

  if (ret < 0) {
    shell_error(sh, "Failed to set value: %d", ret);
    return ret;
  }

  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_kvs,
    SHELL_CMD_ARG(open, NULL, "Open KVS file.", prv_shell_open_file, 2, 0),
    SHELL_CMD_ARG(create, NULL, "Close open file.", prv_shell_create_file, 3,
                  0),
    SHELL_CMD_ARG(set, NULL, "Set value.", prv_shell_set_value, 3, 0),
    SHELL_CMD_ARG(get, NULL, "Get value.", prv_shell_fetch_value, 3, 0),
    SHELL_CMD_ARG(delete, NULL, "Delete value.", prv_shell_delete_value, 2, 0),

    SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(kvs, &sub_kvs, "Key/Value Storage", NULL);

#endif
