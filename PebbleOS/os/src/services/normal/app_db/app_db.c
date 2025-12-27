/*
 * Copyright (C) Ovyl
 */

/**
 * @file app_db.c
 * @author Evan Stoddard
 * @brief
 */

#include "app_db.h"

#include <errno.h>

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>

#include "services/normal/filesystem/pfs.h"
#include "services/normal/kvs_storage/kvs_file.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(app_db);

#define APP_DB_FILE "/pfs/app_db"

// Currently, max file size is ignored
#define APP_DB_FILE_MAX_SIZE_BYTES 1024U

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef AppDB_UUID_Search_Context_t
 * @brief [TODO:description]
 *
 */
typedef struct AppDB_UUID_Search_Context_t {
  Uuid_t *uuid;
  int32_t *app_id;
  bool found;
} AppDB_UUID_Search_Context_t;

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static struct {
  KVS_File_t file;

  struct k_mutex mutex;

  int32_t latest_app_id;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param record_offset [TODO:parameter]
 * @param record_header [TODO:parameter]
 * @param ctx [TODO:parameter]
 * @return [TODO:return]
 */
int prv_enumeration_wrapper_callback(KVS_Iterator_t *iterator, off_t record_offset,
                                     KVS_Record_Header_t *record_header, void *ctx) {
  AppDBEnumerateCallback_t *cb = (AppDBEnumerateCallback_t *)ctx;

  int32_t app_id = 0;
  AppDBEntry_t entry = {0};

  int ret =
      kvs_file_get_key_for_record_offset(&prv_inst.file, record_offset, &app_id, sizeof(app_id));

  if (ret < 0) {
    LOG_ERR("Failed to get key for enumerated record: %d", ret);
    return ret;
  }

  ret = kvs_file_get_value_for_record_offset(&prv_inst.file, record_offset, &entry, sizeof(entry));
  if (ret < 0) {
    LOG_ERR("Failed to get entry for enumerated record: %d", ret);
    return ret;
  }

  cb->callback(&entry, app_id, cb->ctx);

  return 1;
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
int prv_app_id_for_uuid_foreach_callback(KVS_Iterator_t *iterator, off_t record_offset,
                                         KVS_Record_Header_t *record_header, void *ctx) {
  AppDB_UUID_Search_Context_t *_ctx = (AppDB_UUID_Search_Context_t *)ctx;

  AppDBEntry_t entry = {0};
  int32_t app_id = 0;

  int ret =
      kvs_file_get_key_for_record_offset(&prv_inst.file, record_offset, &app_id, sizeof(app_id));
  if (ret < 0) {
    LOG_ERR("Failed to get App ID for record offset: %d", ret);
    return ret;
  }

  ret = kvs_file_get_value_for_record_offset(&prv_inst.file, record_offset, &entry, sizeof(entry));
  if (ret < 0) {
    LOG_ERR("Failed to get app DB entry for record offset: %d", ret);
    return ret;
  }

  if (uuid_equal(&entry.uuid, _ctx->uuid) == false) {
    // Keep on searching
    return 1;
  }

  _ctx->found = true;
  *(_ctx->app_id) = app_id;

  return 0;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int app_db_init(void) {
  int ret = k_mutex_init(&prv_inst.mutex);
  if (ret < 0) {
    return ret;
  }

  ret = kvs_file_open(&prv_inst.file, APP_DB_FILE);

  if (ret < 0) {
    // While not currently implemented, KVS file should self-repair. If self
    // repair fails, then delete corrupted KVS file, and attempt to create a
    // new one.  If failure occurs due to bad flash or some other
    // initialization issue, then creation would also fail.  Probably not the
    // best way to handle all of this, but it was quick and sound enough for
    // MVP.
    if (ret != -ENOENT) {
      // Ignoring return right now.  If the issue is flash or filesystem, then
      // creating will also fail, most likely for similar reasons.  Wanted to
      // avoid extra nesting.  Should probably check this return.
      pfs_delete(APP_DB_FILE);
    }

    ret = kvs_file_create(&prv_inst.file, APP_DB_FILE, APP_DB_FILE_MAX_SIZE_BYTES);

    LOG_ERR("Failed to create App DB file: %d", ret);
    return ret;
  }

  return 0;
}

int app_db_app_id_for_uuid(Uuid_t *uuid, int32_t *app_id) {
  if (uuid == NULL || app_id == NULL) {
    return -EINVAL;
  }

  k_mutex_lock(&prv_inst.mutex, K_FOREVER);

  AppDB_UUID_Search_Context_t context = {.found = false, .app_id = app_id, .uuid = uuid};

  KVS_Record_Foreach_Callback_t callback = {.callback = prv_app_id_for_uuid_foreach_callback,
                                            .ctx = &context};

  int ret = kvs_file_foreach(&prv_inst.file, &callback, &context);

  if (ret < 0) {
    k_mutex_unlock(&prv_inst.mutex);
    return ret;
  }

  if (context.found == false) {
    ret = -ENOENT;
  }

  k_mutex_unlock(&prv_inst.mutex);

  return 0;
}

int32_t app_db_next_app_id(void) {
  return prv_inst.latest_app_id + 1;
}

int app_db_entry_for_uuid(Uuid_t *uuid, AppDBEntry_t *entry) {
  int ret = -ENOTSUP;

  return ret;
}

int app_db_entry_for_app_id(int32_t app_id, AppDBEntry_t *entry) {
  if (entry == NULL) {
    return -EINVAL;
  }

  k_mutex_lock(&prv_inst.mutex, K_FOREVER);

  int ret = kvs_file_get_pair(&prv_inst.file, &app_id, sizeof(app_id), entry, sizeof(AppDBEntry_t));

  k_mutex_unlock(&prv_inst.mutex);

  return ret;
}

int app_db_insert(AppDBEntry_t *entry) {
  if (entry == NULL) {
    return -EINVAL;
  }

  k_mutex_lock(&prv_inst.mutex, K_FOREVER);

  int32_t app_id = 0;

  int ret = app_db_app_id_for_uuid(&entry->uuid, &app_id);

  if (ret == -ENOENT) {
    app_id = app_db_next_app_id();
  } else if (ret < 0) {
    LOG_ERR("Failed to check if previous entry exists for app UUID: %d", ret);
    k_mutex_unlock(&prv_inst.mutex);
    return ret;
  }

  ret = kvs_file_set_pair(&prv_inst.file, &app_id, sizeof(app_id), entry, sizeof(AppDBEntry_t));
  if (ret < 0) {
    LOG_ERR("Failed to insert/update app DB entry: %d", ret);
    k_mutex_unlock(&prv_inst.mutex);
    return ret;
  }

  // NOTE: Would be highly unlikely and crazy wild if this rolled over, but
  // should probably check to ensure this doesn't rollover... Not sure what we'd
  // do in that case since the install ID is sync'd with phone at time of app
  // install... I think...
  prv_inst.latest_app_id++;

  k_mutex_unlock(&prv_inst.mutex);

  return 0;
}

int app_db_delete(int32_t app_id) {
  k_mutex_lock(&prv_inst.mutex, K_FOREVER);

  int ret = kvs_file_delete_pair(&prv_inst.file, &app_id, sizeof(app_id));

  k_mutex_unlock(&prv_inst.mutex);

  return ret;
}

int app_db_enumerate(AppDBEnumerateCallback_t *callback) {
  if (callback == NULL || callback->callback == NULL) {
    return -EINVAL;
  }

  k_mutex_lock(&prv_inst.mutex, K_FOREVER);

  KVS_Record_Foreach_Callback_t wrapper_cb = {.callback = prv_enumeration_wrapper_callback,
                                              .ctx = callback};

  int ret = kvs_file_foreach(&prv_inst.file, &wrapper_cb, callback);

  k_mutex_unlock(&prv_inst.mutex);

  return ret;
}
