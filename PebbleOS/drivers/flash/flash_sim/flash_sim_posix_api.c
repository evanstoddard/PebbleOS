/*
 * Copyright (C) Ovyl
 */

/**
 * @file flash_sim_posix_api.c
 * @author Evan Stoddard
 * @brief POSIX bindings for driver
 */

#include "flash_sim_posix_api.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

int flash_sim_init(flash_sim_data_t *data)
{
    // Open or create binary file to contain contents of external flash
    data->fd = open(data->file_path, O_RDWR | O_CREAT, 0600);
    if (data->fd < 0)
    {
        return -EIO;
    }

    // Ensure the file is the size of the specified flash size defined in the device tree node
    int ret = ftruncate(data->fd, data->size_bytes);
    if (ret != 0)
    {
        return -EIO;
    }

    /*
     * TODO: Ideally, the mapped pointer isn't given write access, as that would
     * would allow modifications to the emulated flash file just by interacting directly
     * with the pointer.  That is rarely the case with actual hardware.  Eventually, a better solution
     * would be to mark this as read only and actually implement the write functions.
     * At the time of writing this driver, it was only used to quickly simulate fetching
     * system resources from memory mapped flash without needing to cache data.  So this worked
     * fine for that purpose.  This should all be changed to better simulate real hardware.
     */

    // Map file to pointer to emulate direct memory access. Not a true analog for hardware but good enough.
    data->mapped = mmap(NULL, data->size_bytes, PROT_WRITE | PROT_READ, MAP_SHARED, data->fd, 0);
    if (data->mapped == MAP_FAILED)
    {
        return -EIO;
    }

    return 0;
}
