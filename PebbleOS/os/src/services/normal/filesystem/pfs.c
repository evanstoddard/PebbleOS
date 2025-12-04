/*
 * Copyright (C) Ovyl
 */

/**
 * @file pfs.c
 * @author Evan Stoddard
 * @brief Filesystem for Pebble (NOTE: This is based on LFS and, therefore, not
 * compatible with the original PFS)
 */

#include "pfs.h"

#include <stdbool.h>

#include <zephyr/logging/log.h>

#include <zephyr/device.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>

#include <zephyr/storage/flash_map.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(pfs);

/*****************************************************************************
 * Variables
 *****************************************************************************/

static struct {
  bool initialized;
} prv_inst;

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

int pfs_init(void) {
  if (prv_inst.initialized == true) {
    return -EALREADY;
  }

  return 0;
}
