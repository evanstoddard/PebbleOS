/*
 * Copyright (C) Ovyl
 */

/**
 * @file kvs_iterator.c
 * @author Evan Stoddard
 * @brief
 */

#include "kvs_iterator.h"

#include <errno.h>

#include <stdbool.h>
#include <stddef.h>

#include <string.h>

#include <zephyr/logging/log.h>

#include <zephyr/sys/crc.h>

#include "services/normal/filesystem/pfs.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(kvs_iterator);

#define KVS_OVERWRITE_IN_PROGRESS_MASK (0x80)
#define KVS_OVERWRITE_COMPLETE_MASK (0x40)

/*****************************************************************************
 * Variables
 *****************************************************************************/

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
static inline uint8_t prv_hash_for_key(const void *key,
                                       const size_t key_len_bytes) {
  if (key == NULL || key_len_bytes == 0) {
    return 0;
  }

  return crc8_rohc(0, key, key_len_bytes);
}

/**
 * @brief [TODO:description]
 *
 * @param record_header [TODO:parameter]
 * @return [TODO:return]
 */
bool prv_record_header_eof_header(KVS_Record_Header_t *record_header) {
  if (record_header->key_size_bytes & KEY_RECORD_KEY_SIZE_EOF_MASK) {
    return true;
  }

  return false;
}

/**
 * @brief [TODO:description]
 *
 * @param record_header [TODO:parameter]
 * @return [TODO:return]
 */
bool prv_record_header_valid_eof_header(KVS_Record_Header_t *record_header) {
  KVS_Record_Header_t valid_header;
  memset(&valid_header, 0xFF, sizeof(KVS_Record_Header_t));

  int ret = memcmp(record_header, &valid_header, sizeof(KVS_Record_Header_t));

  return (ret == 0);
}

/**
 * @brief [TODO:description]
 *
 * @param record_header [TODO:parameter]
 * @return [TODO:return]
 */
bool prv_record_header_valid(KVS_Record_Header_t *record_header,
                             bool *eof_header) {
  *eof_header = false;

  int ret = memcmp(record_header->magic, KVS_FILE_RECORD_HEADER_MAGIC,
                   sizeof(KVS_FILE_RECORD_HEADER_MAGIC));

  if (ret != 0) {
    return false;
  }

  if (prv_record_header_eof_header(record_header) == false) {
    return true;
  }

  *eof_header = true;
  return prv_record_header_valid_eof_header(record_header) == false;
}

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @return [TODO:return]
 */
off_t prv_next_record_offset(KVS_Iterator_t *iterator) {
  if (iterator->current_header_position == 0) {
    return sizeof(KVS_File_Header_t);
  }

  bool eof_header = false;

  // Current header is invalid, so we can't determine next record offset
  if (prv_record_header_valid(&iterator->current_record_header, &eof_header) ==
      false) {
    return -EBADMSG;
  }

  // If we've hit EOF header, then return 0 as this is a special case.
  if (eof_header == true) {
    return 0;
  }

  off_t offset = iterator->current_header_position;
  offset += sizeof(KVS_Record_Header_t);
  offset += iterator->current_record_header.key_size_bytes;
  offset += iterator->current_record_header.value_size_bytes;

  return offset;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int kvs_iterator_init(KVS_Iterator_t *iterator, pfs_file_t *file) {
  if (iterator == NULL || file == NULL) {
    return -EINVAL;
  }

  memset(iterator, 0, sizeof(KVS_Iterator_t));

  iterator->file = file;

  return 0;
}

int kvs_iterator_reset(KVS_Iterator_t *iterator) {
  int ret = pfs_seek(iterator->file, 0, FS_SEEK_SET);

  if (ret < 0) {
    LOG_ERR("Failed to reset KVS file: %d", ret);
    return ret;
  }

  iterator->current_header_position = 0;
  memset(&iterator->current_record_header, 0, sizeof(KVS_Record_Header_t));

  return 0;
}

int kvs_iterator_fetch_file_header(KVS_Iterator_t *iterator) {
  if (iterator == NULL) {
    return -EINVAL;
  }

  int ret = kvs_iterator_reset(iterator);

  if (ret < 0) {
    return ret;
  }

  ssize_t read = pfs_read(iterator->file, &iterator->file_header,
                          sizeof(KVS_File_Header_t));

  if (read < 0) {
    LOG_ERR("Failed to read KVS file header: %d", read);
    return read;
  }

  if (read != sizeof(KVS_File_Header_t)) {
    LOG_ERR("Invalid KVS file header size.");
    return -EMSGSIZE;
  }

  ret = memcmp(iterator->file_header.magic, KVS_FILE_FILE_HEADER_MAGIC,
               sizeof(KVS_FILE_FILE_HEADER_MAGIC));

  if (ret != 0) {
    LOG_ERR("Invalid KVS file header magic.");
    return -EBADMSG;
  }

  return 0;
}

int kvs_iterator_next_record(KVS_Iterator_t *iterator) {
  if (iterator == NULL) {
    return -EINVAL;
  }

  off_t header_offset = prv_next_record_offset(iterator);

  if (header_offset <= 0) {
    return header_offset;
  }

  iterator->current_header_position = header_offset;

  int ret = pfs_seek(iterator->file, header_offset, FS_SEEK_SET);

  if (ret < 0) {
    LOG_ERR("Failed to move to next record: %d", ret);
    return ret;
  }

  ssize_t read = pfs_read(iterator->file, &iterator->current_record_header,
                          sizeof(KVS_Record_Header_t));

  if (read != sizeof(KVS_Record_Header_t)) {
    LOG_ERR("Unable to fetch complete KVS record header: %d", read);
    return -EBADMSG;
  }

  // Validate latest read record header
  bool eof_record = false;
  bool valid =
      prv_record_header_valid(&iterator->current_record_header, &eof_record);

  if (valid == false) {
    return -EBADMSG;
  }

  // If latest read record is EOF record, return 0 to stop iterating
  if (eof_record == true) {
    return 0;
  }

  // Successfully read non-eof record header
  return 1;
}

int kvs_iterator_mark_records_overwrite_begin(KVS_Iterator_t *iterator,
                                              void *key, size_t key_len_bytes) {
  if (iterator == NULL || key == NULL || key_len_bytes == 0) {
    return -EINVAL;
  }

  static uint8_t key_buf[KVS_MAX_KEY_LEN] = {0};

  bool record_found = false;
  uint8_t key_hash = prv_hash_for_key(key, key_len_bytes);

  int ret = kvs_iterator_reset(iterator);
  if (ret < 0) {
    return ret;
  }

  do {
    ret = kvs_iterator_next_record(iterator);

    if (ret > 0) {
      return ret;
    }

    if (ret == 0) {
      break;
    }

    if (iterator->current_record_header.key_size_bytes != key_len_bytes &&
        iterator->current_record_header.key_hash != key_hash) {
      continue;
    }

    ssize_t read = pfs_read(iterator->file, key_buf, key_len_bytes);

    if (read < 0) {
      return read;
    }

    if (read != key_len_bytes) {
      return -EBADMSG;
    }

    int cmp = memcmp(key, key_buf, key_len_bytes);

    if (cmp != 0) {
      continue;
    }

    record_found = true;

    ret = pfs_seek(iterator->file, iterator->current_header_position,
                   FS_SEEK_CUR);
    if (ret < 0) {
      return ret;
    }

    iterator->current_record_header.flags &= ~(KVS_OVERWRITE_IN_PROGRESS_MASK);
    ssize_t written =
        pfs_write(iterator->file, &iterator->current_record_header,
                  sizeof(KVS_Record_Header_t));

    if (written != sizeof(KVS_Record_Header_t)) {
      return -EIO;
    }

  } while (ret > 0);

  if (record_found == false) {
    return -ENOENT;
  }

  return 0;
}

int kvs_iterator_mark_records_overwrite_complete(KVS_Iterator_t *iterator,
                                                 void *key,
                                                 size_t key_len_bytes) {
  if (iterator == NULL || key == NULL || key_len_bytes == 0) {
    return -EINVAL;
  }

  static uint8_t key_buf[KVS_MAX_KEY_LEN] = {0};

  bool record_found = false;
  uint8_t key_hash = prv_hash_for_key(key, key_len_bytes);

  int ret = kvs_iterator_reset(iterator);
  if (ret < 0) {
    return ret;
  }

  do {
    ret = kvs_iterator_next_record(iterator);

    if (ret > 0) {
      return ret;
    }

    if (ret == 0) {
      break;
    }

    if (iterator->current_record_header.key_size_bytes != key_len_bytes &&
        iterator->current_record_header.key_hash != key_hash) {
      continue;
    }

    ssize_t read = pfs_read(iterator->file, key_buf, key_len_bytes);

    if (read < 0) {
      return read;
    }

    if (read != key_len_bytes) {
      return -EBADMSG;
    }

    int cmp = memcmp(key, key_buf, key_len_bytes);

    if (cmp != 0) {
      continue;
    }

    record_found = true;

    ret = pfs_seek(iterator->file, iterator->current_header_position,
                   FS_SEEK_CUR);
    if (ret < 0) {
      return ret;
    }

    iterator->current_record_header.flags &= ~(KVS_OVERWRITE_COMPLETE_MASK);
    ssize_t written =
        pfs_write(iterator->file, &iterator->current_record_header,
                  sizeof(KVS_Record_Header_t));

    if (written != sizeof(KVS_Record_Header_t)) {
      return -EIO;
    }

  } while (ret > 0);

  if (record_found == false) {
    return -ENOENT;
  }

  return 0;
}
