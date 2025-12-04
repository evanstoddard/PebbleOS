/*
 * Copyright (C) Ovyl
 */

/**
 * @file services.c
 * @author Evan Stoddard
 * @brief
 */

#include "services.h"

#include <zephyr/logging/log.h>

#include "normal/normal_services.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(services);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

int services_early_init(void) {
  int ret = normal_services_init_early();

  if (ret < 0) {
    LOG_ERR("Failed to initialize normal early services: %d", ret);
    return ret;
  }

  return 0;
}

int services_init(void) {
  int ret = normal_services_init();

  if (ret < 0) {
    LOG_ERR("Failed to initialize normal services: %d", ret);
    return ret;
  }

  return 0;
}
