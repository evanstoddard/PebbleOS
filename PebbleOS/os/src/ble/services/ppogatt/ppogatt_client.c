/*
 * Copyright (C) Ovyl
 */

/**
 * @file ppogatt_client.c
 * @author Evan Stoddard
 * @brief
 */

#include "ppogatt_client.h"

#include <stddef.h>

#include <zephyr/sys/slist.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Private instance
 */
static struct {
  sys_slist_t clients;
} prv_inst;

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void ppogatt_client_manager_init(void) {
  sys_slist_init(&prv_inst.clients);
}

void ppogatt_client_manager_destroy_all(void) {
  PPoGATT_Client_t *client = (PPoGATT_Client_t *)sys_slist_peek_head(&prv_inst.clients);

  while (client) {
    ppogatt_client_destroy(client);

    client = (PPoGATT_Client_t *)sys_slist_peek_head(&prv_inst.clients);
  }
}

PPoGATT_Client_t *ppogatt_client_create(void) {
  // FIXME: Need to properly handle kernel vs process heap.
  PPoGATT_Client_t *client = (PPoGATT_Client_t *)k_malloc(sizeof(PPoGATT_Client_t));

  if (client == NULL) {
    return NULL;
  }

  memset(client, 0, sizeof(PPoGATT_Client_t));

  sys_slist_prepend(&prv_inst.clients, (sys_snode_t *)client);

  return client;
}

void ppogatt_client_destroy(PPoGATT_Client_t *client) {
  if (client == NULL) {
    return;
  }

  sys_slist_find_and_remove(&prv_inst.clients, (sys_snode_t *)client);

  k_free(client);
}
