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

#include "services/normal/filesystem/pfs.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(kvs_iterator);

#define KVS_OVERWRITE_IN_PROGRESS_MASK (0x80)
#define KVS_OVERWRITE_COMPLETE_MASK (0x40)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @typedef KVS_Filter_Context_t
 * @brief
 *
 */
typedef struct KVS_Filter_Context_t {
  KVS_Record_Filter_t *record_filter;

  KVS_Record_Foreach_Callback_t *target_callback;

  bool first_occurence;

  bool record_found;

  KVS_Record_Header_t record_header;
  off_t record_offset;
} KVS_Filter_Context_t;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

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

/**
 * @brief Clear specified flags for given header offset
 *
 * @param iterator Pointer to iterator
 * @param header_offset Offset of target record
 * @param flags Flags to clear
 * @return [TODO:return]
 */
int prv_clear_record_flags(KVS_Iterator_t *iterator, off_t header_offset,
                           uint8_t flags) {
  int ret = pfs_seek(iterator->file, header_offset, FS_SEEK_SET);
  if (ret < 0) {
    return ret;
  }

  KVS_Record_Header_t header = {0};

  ssize_t bytes = pfs_read(iterator->file, &header, sizeof(header));

  if (bytes < 0) {
    LOG_ERR("Error reading header: %d", bytes);
    return bytes;
  }

  if (bytes != sizeof(header)) {
    LOG_WRN("Potential corrupted header.");
    return -EBADMSG;
  }

  ret = pfs_seek(iterator->file, header_offset, FS_SEEK_SET);
  if (ret < 0) {
    return ret;
  }

  header.flags &= ~(flags);

  bytes = pfs_read(iterator->file, &header, sizeof(header));

  if (bytes < 0) {
    return bytes;
  }

  if (bytes != sizeof(header)) {
    return -EIO;
  }

  return 0;
}

/**
 * @brief
 *
 * @param iterator [TODO:parameter]
 * @param record_offset [TODO:parameter]
 * @param record_header [TODO:parameter]
 * @param ctx [TODO:parameter]
 * @return [TODO:return]
 */
static int prv_filtered_foreach_wrapper(KVS_Iterator_t *iterator,
                                        off_t record_offset,
                                        KVS_Record_Header_t *record_header,
                                        void *ctx) {
  KVS_Filter_Context_t *filter_ctx = (KVS_Filter_Context_t *)ctx;
  KVS_Record_Filter_t *filter = filter_ctx->record_filter;

  uint8_t key_buf[KVS_MAX_KEY_LEN] = {0};

  if (filter->key != NULL) {
    if (record_header->key_size_bytes != filter->key_len) {
      return 1;
    }

    if (record_header->key_hash != filter->key_hash) {
      return 1;
    }

    ssize_t bytes =
        pfs_read(iterator->file, key_buf, record_header->key_size_bytes);

    if (bytes < 0) {
      LOG_ERR("Failed to read key: %d", bytes);
      return bytes;
    }

    if (bytes != record_header->key_size_bytes) {
      LOG_ERR("Key size mismatch.");
      return -EBADMSG;
    }

    int cmp = memcmp(key_buf, filter->key, record_header->key_size_bytes);
    if (cmp != 0) {
      return 1;
    }
  }

  uint8_t flag_mask = ~(filter->flags);
  bool record_match = false;

  if (filter->exact_flag_match && (flag_mask == record_header->flags)) {
    record_match = true;
  } else if (!filter->exact_flag_match && (flag_mask | record_header->flags)) {
    record_match = true;
  }

  if (record_match == false) {
    return 1;
  }

  filter_ctx->record_found = true;
  filter_ctx->record_offset = record_offset;

  if (filter_ctx->target_callback) {
    int ret = filter_ctx->target_callback->callback(
        iterator, record_offset, record_header,
        filter_ctx->target_callback->ctx);

    if (ret < 0) {
      return ret;
    }
  }

  if (filter->exact_flag_match) {
    return 0;
  }

  return 1;
}

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param filter [TODO:parameter]
 * @param callback [TODO:parameter]
 * @param first_occurence [TODO:parameter]
 * @return [TODO:return]
 */
static int prv_filtered_foreach_record(KVS_Iterator_t *iterator,
                                       KVS_Record_Filter_t *filter,
                                       KVS_Record_Foreach_Callback_t *callback,
                                       bool first_occurence) {
  KVS_Filter_Context_t filter_ctx = {0};
  filter_ctx.record_filter = filter;
  filter_ctx.target_callback = callback;
  filter_ctx.first_occurence = first_occurence;

  KVS_Record_Foreach_Callback_t wrapped_callback = {
      .callback = prv_filtered_foreach_wrapper, .ctx = &filter_ctx};

  // Generate hash for provided key
  if (filter->key != NULL) {
    filter->key_hash = kvs_hash_for_key(filter->key, filter->key_len);
  }

  int ret = kvs_iterator_foreach_record(iterator, &wrapped_callback);

  if (ret < 0) {
    return ret;
  }

  if (filter_ctx.record_found == false) {
    return -ENOENT;
  }

  return 1;
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

int kvs_iterator_foreach_record(KVS_Iterator_t *iterator,
                                KVS_Record_Foreach_Callback_t *callback) {
  int ret = kvs_iterator_reset(iterator);
  if (ret < 0) {
    return ret;
  }

  bool cont = true;

  do {
    ret = kvs_iterator_next_record(iterator);

    if (ret < 0) {
      return ret;
    }

    if (ret == 0) {
      break;
    }

    int cb_ret =
        callback->callback(iterator, iterator->current_header_position,
                           &iterator->current_record_header, callback->ctx);

    if (cb_ret < 0) {
      return cb_ret;
    }

    if (cb_ret == 0) {
      cont = 0;
    }

  } while (cont && ret > 0);

  return 0;
}

int kvs_iterator_filtered_foreach_record(
    KVS_Iterator_t *iterator, KVS_Record_Filter_t *filter,
    KVS_Record_Foreach_Callback_t *callback) {
  if (iterator == NULL || filter == NULL || callback == NULL) {
    return -EINVAL;
  }

  return prv_filtered_foreach_record(iterator, filter, callback, false);
}

int kvs_iterator_first_occurence(KVS_Iterator_t *iterator,
                                 KVS_Record_Filter_t *filter,
                                 off_t *record_offset,
                                 KVS_Record_Header_t *record_header) {
  if (iterator == NULL || filter == NULL || record_offset == NULL ||
      record_header == NULL) {
    return -EINVAL;
  }

  int ret = prv_filtered_foreach_record(iterator, filter, NULL, true);

  return ret;
}
