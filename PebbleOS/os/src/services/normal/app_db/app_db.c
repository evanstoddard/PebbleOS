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
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

int app_db_init(void) {
  int ret = kvs_file_open(&prv_inst.file, APP_DB_FILE);

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

    ret = kvs_file_create(&prv_inst.file, APP_DB_FILE,
                          APP_DB_FILE_MAX_SIZE_BYTES);

    LOG_ERR("Failed to create App DB file: %d", ret);
    return ret;
  }

  return 0;
}

int app_db_app_id_for_uuid(Uuid_t *uuid, int32_t *app_id) {
  int ret = -ENOTSUP;

  return ret;
}

int32_t app_db_next_app_id(void) { return prv_inst.latest_app_id + 1; }

int app_db_entry_for_uuid(Uuid_t *uuid, AppDBEntry_t *entry) {
  int ret = -ENOTSUP;

  return ret;
}

int app_db_entry_for_app_id(uint32_t app_id, AppDBEntry_t *entry) {
  int ret = -ENOTSUP;

  return ret;
}

int app_db_insert(AppDBEntry_t *entry) {
  int ret = -ENOTSUP;

  return ret;
}

int app_db_delete(uint32_t app_id) {
  int ret = -ENOTSUP;

  return ret;
}

int app_db_enumerate(AppDBEnumerateCallback_t callback, void *ctx) {
  int ret = -ENOTSUP;

  return ret;
}
