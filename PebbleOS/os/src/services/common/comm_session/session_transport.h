/*
 * Copyright (C) Ovyl
 */

/**
 * @file session_transport.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef session_transport_h
#define session_transport_h

#include <stdbool.h>

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
 * @typedef CommSession_t
 * @brief Forward declaration of comm session
 *
 */
typedef struct CommSession_t CommSession_t;

typedef enum {
  CommSessionTransportType_PlainSPP = 0,
  CommSessionTransportType_iAP = 1,
  CommSessionTransportType_PPoGATT = 2,
  CommSessionTransportType_QEMU = 3,
  CommSessionTransportType_PULSE = 4,
} CommSessionTransportType_t;

typedef enum {
  TRANSPORT_SYSTEM,
  TRANSPORT_APP,
  TRANSPORT_HYBRID,
} Transport_Destination_t;

/**
 * @typedef Transport
 * @brief Opaque definition of transport
 *
 */
typedef struct Transport Transport;

typedef struct TransportImplementation_t {
  void (*send_next)(Transport *transport);
  void (*close)(Transport *transport);
  void (*reset)(Transport *transport);

  const Uuid_t *(*get_uuid)(Transport *transport);
  CommSessionTransportType_t (*get_type)(Transport *transport);

  bool (*schedule)(CommSession_t *session);
  bool (*is_current_task_schedule_task)(Transport *transport);
} TransportImplementation_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param transport [TODO:parameter]
 * @param implementation [TODO:parameter]
 * @param destination [TODO:parameter]
 * @return [TODO:return]
 */
CommSession_t *comm_session_open(Transport *transport,
                                 const TransportImplementation_t *implementation,
                                 Transport_Destination_t destination);

#ifdef __cplusplus
}
#endif
#endif /* session_transport_h */
