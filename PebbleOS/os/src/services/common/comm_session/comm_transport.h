/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file comm_transport.h
 * @author Evan Stoddard
 * @brief API that session transport must adhere to
 */

#ifndef comm_transport_h
#define comm_transport_h

#include <zephyr/sys/uuid.h>

#include "comm_session.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef enum {
  CommSessionCloseReason_UnderlyingDisconnection = 0,
  CommSessionCloseReason_ClosedRemotely = 1,
  CommSessionCloseReason_ClosedLocally = 2,
  CommSessionCloseReason_TransportSpecificBegin = 100,
  CommSessionCloseReason_TransportSpecificEnd = 255,
} CommSessionCloseReason_t;

typedef enum {
  CommSessionTransportType_PlainSPP = 0,
  CommSessionTransportType_iAP = 1,
  CommSessionTransportType_PPoGATT = 2,
  CommSessionTransportType_QEMU = 3,
  CommSessionTransportType_PULSE = 4,
} CommSessionTransportType_t;

/**
 * @typedef CommTransport_t
 * @brief Opaque type for transport
 *
 */
typedef struct CommTransport_t CommTransport_t;

typedef void (*CommTransportSendNext)(CommTransport_t *transport);

typedef void (*CommTransportClose)(CommTransport_t *transport);

typedef void (*CommTransportReset)(CommTransport_t *transport);

typedef const struct uuid *(*CommTransportGetUUID)(CommTransport_t *transport);

typedef CommSessionTransportType_t (*CommTransportGetType)(CommTransport_t *transport);

typedef bool (*CommTransportSchedule)(CommSession_t *session);

typedef bool (*CommTransportScheduleTask)(CommTransport_t *transport);

/**
 * @typedef CommTransportImpl_t
 * @brief Comm transport implementation
 *
 */
typedef struct CommTransportImpl_t {
  CommTransportSendNext send_next;

  CommTransportClose close;
  CommTransportReset reset;

  CommTransportGetUUID get_uuid;

  CommTransportGetType get_type;

  CommTransportSchedule schedule;
  CommTransportScheduleTask is_current_task_schedule_task;
} CommTransportImpl_t;

typedef enum CommTransportDestination_t {
  //! The transport carries Pebble Protocol solely for the "system", for example:
  //! iSPP/iAP with Pebble iOS App.
  CommTransportDestinationSystem,

  //! The transport carries Pebble Protocol solely for a Pebble app, for example:
  //! iSPP/iAP with 3rd party native iOS App and PebbleKit iOS.
  CommTransportDestinationApp,

  //! The transport carries Pebble Protocol for both the "system" and "app", for example:
  //! Plain SPP with Pebble Android App.
  CommTransportDestinationHybrid,
} CommTransportDestination_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Called by transport to open a new session
 *
 * @param transport Pointer to transport instance
 * @param implementation Callbacks/API implementing session transport functions
 * @param destination Destination of transport (akin to port number in TCP)
 * @return Returns pointer to new session (can be NULL)
 */
CommSession_t *comm_session_open(CommTransport_t *transport,
                                 const CommTransportImpl_t *implementation,
                                 CommTransportDestination_t destination);

/**
 * @brief Called by transport to indicate that session needs to be closed
 *
 * @param session Pointer to session
 * @param reason Reason for closure
 */
void comm_session_close(CommSession_t *session, CommSessionCloseReason_t reason);

/**
 * @brief Called by transport to copy received data into receive buffer
 *
 * @param session Pointer to session
 * @param data Data to copy
 * @param data_size Size of data to copy
 */
void comm_session_receive_router_write(CommSession_t *session, const uint8_t *data,
                                       size_t data_size);

//! @note bt_lock() is expected to be taken by the caller!
//! @return The total size in bytes, of all the messages in the queue.

/**
 * @brief Returns to total size in bytes of messages in tx(?) queue
 *
 * @param session Pointer to session
 * @return Total bytes in send(?) queue
 */
size_t comm_session_send_queue_get_length(const CommSession_t *session);

/**
 * @brief Copies bytes from send buffer into another buffer
 *
 * @param session Pointer to session
 * @param start_offset Offset into send buffer
 * @param length Number of bytes to copy
 * @param data_out Pointer to buffer to copy into
 * @return Number of bytes copied
 */
size_t comm_session_send_queue_copy(CommSession_t *session, uint32_t start_offset, size_t length,
                                    uint8_t *data_out);

/**
 * @brief Gets a read pointer and number of bytes that can be read from the read pointer.
 *
 * @param session Pointer to session
 * @param data_out Pointer to write pointer to
 * @return Returns size available to be read at provided pointer
 */
size_t comm_session_send_queue_get_read_pointer(const CommSession_t *session,
                                                const uint8_t **data_out);

/**
 * @brief Consume bytes from queue
 *
 * @param session Pointer to session
 * @param length Number of bytes to consume
 */
void comm_session_send_queue_consume(CommSession_t *session, size_t length);

/**
 * @brief Schedule calling transport's send_next function with the KernelBG
 *
 * @param session Pointer to session
 */
void comm_session_send_next(CommSession_t *session);

#ifdef __cplusplus
}
#endif
#endif /* comm_transport_h */
