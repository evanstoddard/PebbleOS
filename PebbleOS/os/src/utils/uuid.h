/*
 * Copyright (C) Ovyl
 */

/**
 * @file uuid.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef uuid_h
#define uuid_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define UUID_SYSTEM ((Uuid_t){0})

#define UUID_INVALID                                                                            \
  ((Uuid_t){0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
            0xFF, 0xFF})

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef Uuid_t
 * @brief [TODO:description]
 *
 */
typedef struct Uuid_t {
  uint8_t byte0;
  uint8_t byte1;
  uint8_t byte2;
  uint8_t byte3;
  uint8_t byte4;
  uint8_t byte5;
  uint8_t byte6;
  uint8_t byte7;
  uint8_t byte8;
  uint8_t byte9;
  uint8_t byte10;
  uint8_t byte11;
  uint8_t byte12;
  uint8_t byte13;
  uint8_t byte14;
  uint8_t byte15;
} __attribute__((__packed__)) Uuid_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param first [TODO:parameter]
 * @param second [TODO:parameter]
 * @return [TODO:return]
 */
static inline bool uuid_equal(Uuid_t *first, Uuid_t *second) {
  if (first == NULL || second == NULL) {
    return false;
  }

  return (memcmp(first, second, sizeof(Uuid_t)) == 0);
}

static inline bool uuid_is_system(Uuid_t *uuid) {
  Uuid_t system_uuid = UUID_SYSTEM;

  return (memcmp(uuid, &system_uuid, sizeof(Uuid_t)) == 0);
}

#ifdef __cplusplus
}
#endif
#endif /* uuid_h */
