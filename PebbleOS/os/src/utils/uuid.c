/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file uuid.c
 * @author Evan Stoddard
 * @brief Collection of UUID definitions and helper functions
 */

#include "uuid.h"

#include <string.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

static const struct uuid prv_system_uuid = UUID_SYSTEM;

static const struct uuid prv_invalid_uuid = UUID_INVALID;

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

bool uuid_is_system(struct uuid *uuid) {
  return (memcmp(uuid, &prv_system_uuid, sizeof(struct uuid)) == 0);
}

bool uuid_is_invalid(struct uuid *uuid) {
  return (memcmp(uuid, &prv_invalid_uuid, sizeof(struct uuid)) == 0);
};
