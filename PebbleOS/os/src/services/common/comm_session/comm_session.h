/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file comm_session.h
 * @author Evan Stoddard
 * @brief Abstracts Pebble Protocol away from transport medium
 */

#ifndef comm_session_h
#define comm_session_h

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
 * @typedef CommSession
 * @brief Opaque declaration of comm session
 *
 */
typedef struct CommSession_t CommSession_t;

typedef enum {
  CommSessionTypeInvalid = -1,
  CommSessionTypeSystem = 0,
  CommSessionTypeApp = 1,
  NumCommSessions,
} CommSessionType_t;

typedef enum {
  CommSessionRunState = 1 << 0,
  CommSessionInfiniteLogDumping = 1 << 1,
  CommSessionExtendedMusicService = 1 << 2,
  CommSessionExtendedNotificationService = 1 << 3,
  CommSessionLanguagePackSupport = 1 << 4,
  CommSessionAppMessage8kSupport = 1 << 5,
  CommSessionActivityInsightsSupport = 1 << 6,
  CommSessionVoiceApiSupport = 1 << 7,
  CommSessionSendTextSupport = 1 << 8,
  CommSessionNotificationFilteringSupport = 1 << 9,
  CommSessionUnreadCoredumpSupport = 1 << 10,
  CommSessionWeatherAppSupport = 1 << 11,
  CommSessionRemindersAppSupport = 1 << 12,
  CommSessionWorkoutAppSupport = 1 << 13,
  CommSessionSmoothFwInstallProgressSupport = 1 << 14,
  CommSessionOutOfRange
} CommSessionCapability_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize comm session module (not initializing a session)
 */
void comm_session_init(void);

/**
 * @brief Returns pointer to comm session strictly for system use
 *
 * @return Pointer to session (can be NULL at any time)
 */
CommSession_t *comm_session_get_system_session(void);

#ifdef __cplusplus
}
#endif
#endif /* comm_session_h */
