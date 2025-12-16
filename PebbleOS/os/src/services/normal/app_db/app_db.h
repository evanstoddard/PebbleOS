/*
 * Copyright (C) Ovyl
 */

/**
 * @file app_db.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef app_db_h
#define app_db_h

#include <stdint.h>

#include "utils/internal_version.h"
#include "utils/uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef AppDBEntry_t
 * @brief [TODO:description]
 *
 */
typedef struct AppDBEntry_t {
  Uuid_t uuid;

  uint32_t info_flags;

  uint32_t icon_resource_id;

  Version_t app_version;
  Version_t sdk_version;

  // Currently just uint8_t to get proper alignment until color types are
  // brought in
  uint8_t app_face_bg_color;

  uint8_t template_id;

  // Currently defined with Kconfig.  Will most likely move this define
  // elsewhere.  Original code had this defined somewhere I didn't agree with.
  char name[CONFIG_APP_NAME_SIZE_BYTES];
} __attribute__((__packed__)) AppDBEntry_t;

/**
 * @typedef AppDBEnumerateCallback_t
 * @brief [TODO:description]
 *
 */
typedef struct AppDBEnumerateCallback_t {
  void (*callback)(AppDBEntry_t *entry, int32_t app_id, void *ctx);
  void *ctx;
} AppDBEnumerateCallback_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @return [TODO:return]
 */
int app_db_init(void);

/**
 * @brief [TODO:description]
 *
 * @return [TODO:return]
 */
int32_t app_db_next_app_id(void);

/**
 * @brief [TODO:description]
 *
 * @param uuid [TODO:parameter]
 * @param app_id [TODO:parameter]
 * @return [TODO:return]
 */
int app_db_app_id_for_uuid(Uuid_t *uuid, int32_t *app_id);

/**
 * @brief [TODO:description]
 *
 * @param uuid [TODO:parameter]
 * @param entry [TODO:parameter]
 * @return [TODO:return]
 */
int app_db_entry_for_uuid(Uuid_t *uuid, AppDBEntry_t *entry);

/**
 * @brief [TODO:description]
 *
 * @param app_id [TODO:parameter]
 * @param entry [TODO:parameter]
 * @return [TODO:return]
 */
int app_db_entry_for_app_id(int32_t app_id, AppDBEntry_t *entry);

/**
 * @brief [TODO:description]
 *
 * @param entry [TODO:parameter]
 * @return [TODO:return]
 */
int app_db_insert(AppDBEntry_t *entry);

/**
 * @brief [TODO:description]
 *
 * @param app_id [TODO:parameter]
 * @return [TODO:return]
 */
int app_db_delete(int32_t app_id);

/**
 * @brief [TODO:description]
 *
 * @param callback [TODO:parameter]
 * @return [TODO:return]
 */
int app_db_enumerate(AppDBEnumerateCallback_t *callback);

#ifdef __cplusplus
}
#endif
#endif /* app_db_h */
