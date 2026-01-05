/*
 * Copyright (C) Ovyl
 */

/**
 * @file comm_session.h
 * @author Evan Stoddard
 * @brief
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
 * @typedef CommSession_t
 * @brief Forward declaration of comm session
 *
 */
typedef struct CommSession_t CommSession_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

void comm_session_init(void);

#ifdef __cplusplus
}
#endif
#endif /* comm_session_h */
