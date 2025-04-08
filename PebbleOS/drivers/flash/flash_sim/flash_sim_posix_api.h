/*
 * Copyright (C) Ovyl
 */

/**
 * @file flash_sim_posix_api.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef flash_sim_posix_api_h
#define flash_sim_posix_api_h

#include <stddef.h>

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
 * @typedef flash_sim_data_t
 * @brief [TODO:description]
 *
 */
typedef struct flash_sim_data_t
{
    char *file_path;
    int fd;
    size_t size_bytes;
    void *mapped;
} flash_sim_data_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

int flash_sim_init(flash_sim_data_t *data);

#ifdef __cplusplus
}
#endif
#endif /* flash_sim_posix_api_h */
