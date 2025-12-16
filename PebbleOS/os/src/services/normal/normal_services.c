/*
 * Copyright (C) Ovyl
 */

/**
 * @file normal_services.c
 * @author Evan Stoddard
 * @brief
 */

#include "normal_services.h"

#include <zephyr/logging/log.h>

#include "app_db/app_db.h"
#include "filesystem/pfs.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(normal_services);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

int normal_services_init_early(void) {
  int ret = pfs_init();

  if (ret < 0) {
    LOG_ERR("Failed to initialize PFS: %d", ret);
    return ret;
  }

  return 0;
}

int normal_services_init(void) {
  int ret = app_db_init();
  if (ret < 0) {
    LOG_ERR("Failed to initialize App DB: %d", ret);
    return ret;
  }

  return 0;
}
