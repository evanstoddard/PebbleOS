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

#include <zephyr/logging/log.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include <zephyr/sys/slist.h>

#include "ble/services/ppogatt/ppogatt_protocol_definitions.h"
#include "ppogatt_client_internal.h"

#include "ble/ble_conn_mgr.h"

#include "services/common/comm_session/comm_session.h"
#include "services/common/comm_session/session_send_queue.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(ppogatt_client);

/* PPoGATT Service UUID: 10000000-328E-0FBB-C642-1AA6699BDADA */
#define BT_UUID_PPOGATT_SVC_VAL \
  BT_UUID_128_ENCODE(0x10000000, 0x328E, 0x0FBB, 0xC642, 0x1AA6699BDADA)
#define BT_UUID_PPOGATT_SVC BT_UUID_DECLARE_128(BT_UUID_PPOGATT_SVC_VAL)

/* PPoGATT Data Characteristic UUID: 10000001-328E-0FBB-C642-1AA6699BDADA */
#define BT_UUID_PPOGATT_DATA_VAL \
  BT_UUID_128_ENCODE(0x10000001, 0x328E, 0x0FBB, 0xC642, 0x1AA6699BDADA)

/* PPoGATT Meta Characteristic UUID: 10000002-328E-0FBB-C642-1AA6699BDADA */
#define BT_UUID_PPOGATT_META_VAL \
  BT_UUID_128_ENCODE(0x10000002, 0x328E, 0x0FBB, 0xC642, 0x1AA6699BDADA)

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef enum {
  PPOGATT_CHAR_DATA,
  PPOGATT_CHAR_META,
  PPOGATT_CHAR_COUNT,
} PPoGATT_Char_Idx_t;

/**
 * @brief [TODO:description]
 */
static struct bt_uuid_128 prv_ppogatt_data_uuid = BT_UUID_INIT_128(BT_UUID_PPOGATT_DATA_VAL);

/**
 * @brief [TODO:description]
 */
static struct bt_uuid_128 prv_ppogatt_meta_uuid = BT_UUID_INIT_128(BT_UUID_PPOGATT_META_VAL);

/**
 * @brief [TODO:description]
 */
static const struct bt_uuid *prv_ppogatt_char_uuids[PPOGATT_CHAR_COUNT] = {
    [PPOGATT_CHAR_DATA] = (const struct bt_uuid *)&prv_ppogatt_data_uuid,
    [PPOGATT_CHAR_META] = (const struct bt_uuid *)&prv_ppogatt_meta_uuid,
};

/**
 * @brief Private instance
 */
static struct {
  sys_slist_t clients;

  struct bt_conn *conn;
} prv_inst;

/*****************************************************************************
 * Private Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static void prv_destroy_client(PPoGATT_Client_t *client);

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param params [TODO:parameter]
 * @param buf [TODO:parameter]
 * @param len [TODO:parameter]
 * @return [TODO:return]
 */
static uint8_t prv_notify_cb(struct bt_conn *conn, struct bt_gatt_subscribe_params *params,
                             const void *buf, uint16_t len);

/*****************************************************************************
 * Transport Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param transport [TODO:parameter]
 */
static void prv_transport_send_next(Transport *transport);

/**
 * @brief [TODO:description]
 *
 * @param transport [TODO:parameter]
 */
static void prv_transport_close(Transport *transport);

/**
 * @brief [TODO:description]
 *
 * @param transport [TODO:parameter]
 */
static void prv_transport_reset(Transport *transport);

/**
 * @brief [TODO:description]
 *
 * @param transport [TODO:parameter]
 * @return [TODO:return]
 */
static const Uuid_t *prv_transport_get_uuid(Transport *transport);

/**
 * @brief [TODO:description]
 *
 * @param transport [TODO:parameter]
 * @return [TODO:return]
 */
static CommSessionTransportType_t prv_transport_get_type(Transport *transport);

/**
 * @brief [TODO:description]
 *
 * @param transport [TODO:parameter]
 * @return [TODO:return]
 */
static bool prv_transport_is_current_task_schedule_task(Transport *transport);

/**
 * @brief Transport implementation bindings
 */
static const TransportImplementation_t prv_transport_implementation = {
    .send_next = prv_transport_send_next,
    .close = prv_transport_close,
    .reset = prv_transport_reset,
    .get_uuid = prv_transport_get_uuid,
    .get_type = prv_transport_get_type,
    .is_current_task_schedule_task = prv_transport_is_current_task_schedule_task};

/*****************************************************************************
 * Private Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 */
static void prv_send_next_packets(PPoGATT_Client_t *client);

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @param header [TODO:parameter]
 */
static void prv_handle_reset_complete_packet(PPoGATT_Client_t *client,
                                             PPoGATT_Packet_Header_t *header) {
  if (client->state != PPoGATT_CLIENT_STATE_CONNECTED_CLOSED_AWAIT_SELF_RESET_COMPLETE) {
    /* FIXME: Destroy Client? */
    return;
  }

  client->comm_session =
      comm_session_open((Transport *)client, &prv_transport_implementation, client->transport_dest);

  if (client->comm_session == NULL) {
    LOG_ERR("Failed to open comm session.");
    prv_destroy_client(client);
    return;
  }

  LOG_INF("Comm session created!");
  client->state = PPoGATT_CLIENT_STATE_CONNECTED_OPEN;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 */
static void prv_write_reset_packet(PPoGATT_Client_t *client) {
  PPoGATT_Reset_Packet_t packet;
  packet.header.type = PPoGATT_PACKET_TYPE_RESET;
  packet.header.sn = 0;
  packet.version = 1;
  memcpy(packet.serial_num, "0123456789AB", 12);

  int ret = bt_gatt_write_without_response(prv_inst.conn, client->ble_handles.data_handle, &packet,
                                           sizeof(packet), false);

  if (ret < 0) {
    LOG_ERR("Failed to write reset packet: %d", ret);
    prv_destroy_client(client);
    return;
  }

  client->state = PPoGATT_CLIENT_STATE_CONNECTED_CLOSED_AWAIT_SELF_RESET_COMPLETE;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @param self_initiated [TODO:parameter]
 */
static void prv_enter_reset_state(PPoGATT_Client_t *client, bool self_initiated) {
  client->recv_ctx.next_expected_sn = 0;

  if (self_initiated) {
    client->write_ctx.reset_packet.byte = true;
    client->state = PPoGATT_CLIENT_STATE_CONNECTED_CLOSED_AWAIT_SELF_RESET_COMPLETE_STALLED;
  }

  prv_send_next_packets(client);
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 */
static void prv_start_reset(PPoGATT_Client_t *client) {
  // TODO: A whole bunch of other stuff

  prv_enter_reset_state(client, true);
}

/**
 * @brief Callback when subscription completes
 *
 * @param conn Connection
 * @param err Error code (0 = success)
 * @param params Subscribe params
 */
static void prv_subscribe_cb(struct bt_conn *conn, uint8_t err,
                             struct bt_gatt_subscribe_params *params) {
  PPoGATT_Client_t *client = CONTAINER_OF(params, PPoGATT_Client_t, subscribe_params);

  if (err) {
    LOG_ERR("Subscribe failed: %u", err);
    prv_destroy_client(client);
    return;
  }

  LOG_INF("Subscribed to data characteristic");

  prv_start_reset(client);
}

/**
 * @brief Subscribe to data characteristic notifications
 *
 * @param client PPoGATT client instance
 */
static void prv_client_subscribe(PPoGATT_Client_t *client) {
  memset(&client->subscribe_params, 0, sizeof(client->subscribe_params));
  memset(&client->ccc_disc_params, 0, sizeof(client->ccc_disc_params));

  client->subscribe_params.notify = prv_notify_cb;
  client->subscribe_params.subscribe = prv_subscribe_cb;
  client->subscribe_params.value_handle = client->ble_handles.data_handle;
  client->subscribe_params.end_handle = client->ble_handles.service_end_handle;
  client->subscribe_params.disc_params = &client->ccc_disc_params;
  client->subscribe_params.ccc_handle = 0; /* Auto-discover CCCD */
  client->subscribe_params.value = BT_GATT_CCC_NOTIFY;

  int ret = bt_gatt_subscribe(prv_inst.conn, &client->subscribe_params);

  if (ret < 0) {
    LOG_ERR("Failed to subscribe to data characteristic: %d", ret);
    prv_destroy_client(client);
    return;
  }

  client->state = PPoGATT_CLIENT_STATE_DISCONNECTED_SUBSCRIBING_DATA;
}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param err [TODO:parameter]
 * @param params [TODO:parameter]
 * @param data [TODO:parameter]
 * @param len [TODO:parameter]
 * @return [TODO:return]
 */
static uint8_t prv_on_meta_read(struct bt_conn *conn, uint8_t err,
                                struct bt_gatt_read_params *params, const void *data,
                                uint16_t len) {
  PPoGATT_Client_t *client = CONTAINER_OF(params, PPoGATT_Client_t, read_params);

  if (err != 0) {
    LOG_ERR("Error reading meta: %u", err);
    return BT_GATT_ITER_STOP;
  }

  PPoGATT_Meta_t *meta = (PPoGATT_Meta_t *)data;

  LOG_INF(
      "Fetched PPoGATT Session Meta:\r\n"
      "\tMin Version: %u\r\n"
      "\tMax Version: %u\r\n",
      meta->min_version, meta->max_version);

  memcpy(&client->app_uuid, &meta->app_uuid, sizeof(Uuid_t));

  if (uuid_is_system(&client->app_uuid) == true) {
    client->transport_dest = TRANSPORT_SYSTEM;
  } else {
    client->transport_dest = TRANSPORT_APP;
  }

  prv_client_subscribe(client);

  return BT_GATT_ITER_STOP;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @return [TODO:return]
 */
static int prv_fetch_metadata(PPoGATT_Client_t *client) {
  client->read_params.handle_count = 1;
  client->read_params.single.offset = 0;
  client->read_params.single.handle = client->ble_handles.meta_handle;
  client->read_params.func = prv_on_meta_read;

  int ret = bt_gatt_read(prv_inst.conn, &client->read_params);

  if (ret < 0) {
    LOG_ERR("Failed to queue meta read: %d", ret);
    prv_destroy_client(client);
  }

  return ret;
}

/**
 * @brief [TODO:description]
 *
 * @return [TODO:return]
 */
static PPoGATT_Client_t *prv_create_client(void) {
  // FIXME: Need to properly handle kernel vs process heap.
  PPoGATT_Client_t *client = (PPoGATT_Client_t *)k_malloc(sizeof(PPoGATT_Client_t));

  if (client == NULL) {
    return NULL;
  }

  memset(client, 0, sizeof(PPoGATT_Client_t));

  sys_slist_prepend(&prv_inst.clients, (sys_snode_t *)client);

  return client;
}

/**
 * @brief [TODO:description]
 */
static void prv_destroy_client(PPoGATT_Client_t *client) {
  sys_slist_find_and_remove(&prv_inst.clients, (sys_snode_t *)client);

  k_free(client);
}

/**
 * @brief [TODO:description]
 */
static void prv_destroy_all_clients(void) {
  PPoGATT_Client_t *client = (PPoGATT_Client_t *)sys_slist_peek_head(&prv_inst.clients);

  while (client) {
    prv_destroy_client(client);

    client = (PPoGATT_Client_t *)sys_slist_peek_head(&prv_inst.clients);
  }
}

static uint8_t prv_notify_cb(struct bt_conn *conn, struct bt_gatt_subscribe_params *params,
                             const void *buf, uint16_t len) {
  if (buf == NULL) {
    LOG_INF("Unsubscribed from notifications.");
    return BT_GATT_ITER_STOP;
  }

  PPoGATT_Packet_Header_t *header = (PPoGATT_Packet_Header_t *)buf;

  PPoGATT_Client_t *client = CONTAINER_OF(params, PPoGATT_Client_t, subscribe_params);

  switch (header->type) {
    case PPoGATT_PACKET_TYPE_RESET:
      LOG_INF("Received reset packet.");
      break;
    case PPoGATT_PACKET_TYPE_DATA:
      LOG_INF("Received data packet.");
      break;
    case PPoGATT_PACKET_TYPE_ACK:
      LOG_INF("Received ACK packet.");
      break;
    case PPoGATT_PACKET_TYPE_RESET_COMPLETE:
      prv_handle_reset_complete_packet(client, header);
      break;
    default:
      LOG_ERR("Received unexpected packet type: 0x%02X", header->type);
      /*
       * FIXME: Destroy client?
       */
      break;
  }

  return BT_GATT_ITER_CONTINUE;
}

/**
 * @brief Callback when PPoGATT service is discovered
 *
 * @param handles Array of characteristic handles
 * @param service_end_handle End handle of the service (for CCCD discovery)
 */
static void prv_on_service_discovered(uint16_t *handles, uint16_t service_end_handle) {
  PPoGATT_Client_t *client = prv_create_client();

  if (client == NULL) {
    LOG_ERR("Failed to create client.");
    return;
  }

  client->ble_handles.data_handle = handles[PPOGATT_CHAR_DATA];
  client->ble_handles.meta_handle = handles[PPOGATT_CHAR_META];
  client->ble_handles.service_end_handle = service_end_handle;

  prv_fetch_metadata(client);
}

/**
 * @brief [TODO:description]
 */
static void prv_on_service_removed(void) {
  prv_destroy_all_clients();
}

/**
 * @brief [TODO:description]
 */
static void prv_on_invalidate_all(void) {
  prv_destroy_all_clients();
}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param err [TODO:parameter]
 */
static void prv_on_connected(struct bt_conn *conn, uint8_t err) {
  if (err) {
    LOG_ERR("Connection failed: %d", err);
    return;
  }

  prv_inst.conn = bt_conn_ref(conn);
}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param err [TODO:parameter]
 */
static void prv_on_disconnected(struct bt_conn *conn, uint8_t err) {
  if (prv_inst.conn == NULL) {
    return;
  }

  bt_conn_unref(prv_inst.conn);
  prv_inst.conn = NULL;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @return [TODO:return]
 */
static bool prv_client_supports_enhanced_throughput(PPoGATT_Client_t *client) {
  return (client->version >= 1);
}

/**
 * @brief [TODO:description]
 *
 * @param sn_begin_incl [TODO:parameter]
 * @param sn_end_excl [TODO:parameter]
 * @return [TODO:return]
 */
static uint32_t prv_sn_distance(uint8_t sn_begin_incl, uint32_t sn_end_excl) {
  return ((uint32_t)PPOGATT_SN_MOD_DIV + sn_end_excl - sn_begin_incl) % PPOGATT_SN_MOD_DIV;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @return [TODO:return]
 */
static uint32_t prv_num_packets_in_flight(const PPoGATT_Client_t *client) {
  return prv_sn_distance(client->write_ctx.next_expected_ack_sn, client->write_ctx.next_data_sn);
}

/**
 * @brief [TODO:description]
 *
 * @param current_sn [TODO:parameter]
 * @return [TODO:return]
 */
static uint32_t prv_next_sn(uint32_t current_sn) {
  return (current_sn + 1) % PPOGATT_SN_MOD_DIV;
}

/**
 * @brief [TODO:description]
 *
 * @param sn [TODO:parameter]
 * @return [TODO:return]
 */
static uint32_t prv_prev_sn(uint32_t sn) {
  return ((PPOGATT_SN_MOD_DIV + sn - 1) % PPOGATT_SN_MOD_DIV);
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @return [TODO:return]
 */
static uint16_t prv_get_max_payload_size(const PPoGATT_Client_t *client) {
  size_t size = 0;

  // TODO: Fetch actual GATT MTU Size
  size = 251;

  size -= sizeof(PPoGATT_Packet_Header_t);

  return size;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @param sn [TODO:parameter]
 * @return [TODO:return]
 */
static uint16_t prv_get_payload_size_for_sn(const PPoGATT_Client_t *client, uint32_t sn) {
  return client->write_ctx.payload_sizes[sn];
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @param sn [TODO:parameter]
 * @return [TODO:return]
 */
static bool prv_is_packet_with_sn_awaiting_ack(const PPoGATT_Client_t *client, uint32_t sn) {
  return (prv_get_payload_size_for_sn(client, sn) != 0);
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @param sn_end_excl [TODO:parameter]
 * @return [TODO:return]
 */
static uint16_t prv_total_num_bytes_awaiting_ack_up_to(const PPoGATT_Client_t *client,
                                                       uint32_t sn_end_excl) {
  uint16_t num_bytes = 0;
  for (uint32_t sn = client->write_ctx.next_expected_ack_sn; sn != sn_end_excl;
       sn = prv_next_sn(sn)) {
    num_bytes += prv_get_payload_size_for_sn(client, sn);
  }
  return num_bytes;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @return [TODO:return]
 */
static uint16_t prv_total_num_bytes_awaiting_ack(const PPoGATT_Client_t *client) {
  return prv_total_num_bytes_awaiting_ack_up_to(client, client->write_ctx.next_data_sn);
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @param sn [TODO:parameter]
 * @param payload_size [TODO:parameter]
 */
static void prv_set_payload_size_for_sn(PPoGATT_Client_t *client, uint32_t sn,
                                        uint16_t payload_size) {
  client->write_ctx.payload_sizes[sn] = payload_size;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @param sn_end_excl [TODO:parameter]
 */
static void prv_clear_payload_sizes_up_to(PPoGATT_Client_t *client, uint32_t sn_end_excl) {
  for (uint32_t sn = client->write_ctx.next_expected_ack_sn; sn != sn_end_excl;
       sn = prv_next_sn(sn)) {
    prv_set_payload_size_for_sn(client, sn, 0);
  }
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @param heap_packet_in_out [TODO:parameter]
 * @return [TODO:return]
 */
static PPoGATT_Packet_t *prv_lazily_allocate_packet_if_needed(
    const PPoGATT_Client_t *client, PPoGATT_Packet_t **heap_packet_in_out) {
  PPoGATT_Packet_t *packet = *heap_packet_in_out;

  if (packet) {
    return packet;
  }

  const uint16_t max_payload_size = prv_get_max_payload_size(client);
  if (max_payload_size == 0) {
    return NULL;
  }

  // FIXME: Again, k_malloc definitely a bad idea since it's not thread safe.  Will need to double
  // check calling context and either guard system heap or use dedicated k_heap
  packet = (PPoGATT_Packet_t *)k_malloc(sizeof(PPoGATT_Packet_t) + max_payload_size);
  *heap_packet_in_out = packet;

  return packet;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @param packet_out [TODO:parameter]
 * @param payload_size_out [TODO:parameter]
 * @return [TODO:return]
 */
static const PPoGATT_Packet_t *prv_prepare_reset_packet(PPoGATT_Client_t *client,
                                                        PPoGATT_Packet_t **packet_out,
                                                        uint16_t *payload_size_out) {
  return NULL;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @param packet_out [TODO:parameter]
 * @param payload_size_out [TODO:parameter]
 * @return [TODO:return]
 */
static const PPoGATT_Packet_t *prv_prepare_next_packet(PPoGATT_Client_t *client,
                                                       PPoGATT_Packet_t **packet_out,
                                                       uint16_t *payload_size_out) {
  if (client->write_ctx.reset_packet.byte) {
    return prv_prepare_reset_packet(client, packet_out, payload_size_out);
  }

  if (client->write_ctx.ack_packet.byte) {
    if (prv_client_supports_enhanced_throughput(client) == false) {
      client->write_ctx.send_rx_ack_now = true;
    } else {
      client->write_ctx.outstanding_rx_ack_count++;
      if ((++client->write_ctx.outstanding_rx_ack_count) > (client->write_ctx.rx_window_size / 2)) {
        client->write_ctx.send_rx_ack_now = true;
      }
    }

    if (client->write_ctx.send_rx_ack_now == true) {
      // TODO: Handle ACK timer stuff...

      *payload_size_out = 0;
      return (PPoGATT_Packet_t *)&client->write_ctx.ack_packet.packet;
    }
  }

  if (client->state != PPoGATT_CLIENT_STATE_CONNECTED_OPEN) {
    return NULL;
  }

  if (prv_num_packets_in_flight(client) >= client->write_ctx.tx_window_size) {
    return NULL;
  }

  uint16_t read_space = comm_session_send_queue_get_length(client->comm_session);
  if (read_space == 0) {
    return NULL;
  }

  uint16_t max_payload_size = prv_get_max_payload_size(client);
  if (max_payload_size == 0) {
    return NULL;
  }

  uint16_t offset = prv_total_num_bytes_awaiting_ack(client);
  uint16_t payload_size = prv_get_payload_size_for_sn(client, client->write_ctx.next_data_sn);

  if (payload_size == 0) {
    payload_size = read_space - offset;

    if (payload_size == 0) {
      return NULL;
    }

    payload_size = MIN(payload_size, max_payload_size);
  }

  PPoGATT_Packet_t *packet = prv_lazily_allocate_packet_if_needed(client, packet_out);

  if (!packet) {
    return NULL;
  }

  packet->header.type = PPoGATT_PACKET_TYPE_DATA;
  packet->header.sn = client->write_ctx.next_data_sn;

  comm_session_send_queue_copy(client->comm_session, offset, payload_size, packet->payload);
  *payload_size_out = payload_size;

  return packet;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 */
static void prv_send_next_packets_async(PPoGATT_Client_t *client) {
  comm_session_send_next(client->comm_session);
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @param payload_size [TODO:parameter]
 */
static void prv_finalize_queued_packet(PPoGATT_Client_t *client, uint16_t payload_size) {
  if (client->write_ctx.reset_packet.byte != 0) {
    client->write_ctx.reset_packet.byte = 0;
  } else if (client->write_ctx.send_rx_ack_now && client->write_ctx.ack_packet.byte != 0) {
    client->write_ctx.ack_packet.byte = 0;
    client->write_ctx.send_rx_ack_now = false;
    client->write_ctx.outstanding_rx_ack_count = 0;
  } else {  // we are sending a data packet
    const uint32_t sn = client->write_ctx.next_data_sn;
    prv_set_payload_size_for_sn(client, sn, payload_size);

    /* if (client->write_ctx.ack_timeout_state == AckTimeoutState_Inactive) { */
    /*   prv_reset_ack_timeout(client);  // Enable timeout if we don't already have it set */
    /* } */

    client->write_ctx.next_data_sn = prv_next_sn(sn);
  }
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 */
static void prv_send_next_packets(PPoGATT_Client_t *client) {
  uint16_t payload_size = 0;

  const PPoGATT_Packet_t *packet = NULL;
  PPoGATT_Packet_t *heap_packet = NULL;

  // Cap the number of times we loop here, to avoid blocking the task for too long.
  uint8_t loop_count = 0;

  while ((packet = prv_prepare_next_packet(client, &heap_packet, &payload_size))) {
    ++loop_count;

    int ret = bt_gatt_write_without_response(prv_inst.conn, client->ble_handles.data_handle, packet,
                                             sizeof(PPoGATT_Packet_t) + payload_size, false);

    if (ret != 0) {
      break;
    }

    // Packet successfully queued
    prv_finalize_queued_packet(client, payload_size);
    const uint8_t max_loop_count = 10;
    if (loop_count > max_loop_count) {
      // If more bytes left to send (but loop_count became >= 10),
      // schedule a callback to process them later to avoid blocking the task for too long:
      prv_send_next_packets_async(client);
      break;
    }
  }

  if (heap_packet) {
    k_free(heap_packet);
  }
}

/*****************************************************************************
 * Transport Functions
 *****************************************************************************/

static void prv_transport_send_next(Transport *transport) {}

static void prv_transport_close(Transport *transport) {
  // TODO: Do this
}

static void prv_transport_reset(Transport *transport) {
  // TODO: Do this
}

static const Uuid_t *prv_transport_get_uuid(Transport *transport) {
  return &((PPoGATT_Client_t *)transport)->app_uuid;
}

static CommSessionTransportType_t prv_transport_get_type(Transport *transport) {
  return CommSessionTransportType_PPoGATT;
}

static bool prv_transport_is_current_task_schedule_task(Transport *transport) {
  // TODO: Do this
  return false;
}

/*****************************************************************************
 * BLE GATT Client Definition
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static ble_client_t prv_ble_client = {
    .client_name = "PPoGATT",
    .service_uuid = BT_UUID_PPOGATT_SVC,
    .char_uuids = prv_ppogatt_char_uuids,
    .num_chars = PPOGATT_CHAR_COUNT,

    .on_service_discovered = prv_on_service_discovered,
    .on_service_removed = prv_on_service_removed,
    .on_invalidate_all = prv_on_invalidate_all,
};

BT_CONN_CB_DEFINE(prv_bt_callbacks) = {
    .connected = prv_on_connected,
    .disconnected = prv_on_disconnected,
};

/*****************************************************************************
 * Functions
 *****************************************************************************/

void ppogatt_client_init(void) {
  sys_slist_init(&prv_inst.clients);

  ble_conn_mgr_register_client(&prv_ble_client);
}

void ppogatt_client_send_next(struct Transport *transport) {
  if (transport == NULL) {
    return;
  }

  prv_send_next_packets((PPoGATT_Client_t *)transport);
}
