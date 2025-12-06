/*
 * Copyright (C) Ovyl
 */

/**
 * @file pfs.c
 * @author Evan Stoddard
 * @brief Filesystem for Pebble (NOTE: This is based on LFS and, therefore, not
 * compatible with the original PFS)
 */

#include "pfs.h"

#include <stdbool.h>

#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include <zephyr/device.h>
#include <zephyr/fs/littlefs.h>

#include <zephyr/storage/flash_map.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(pfs);

#define PFS_FSTAB_NODE DT_NODELABEL(pfs);

/*****************************************************************************
 * Variables
 *****************************************************************************/

static struct {
  bool initialized;
} prv_inst;

FS_FSTAB_DECLARE_ENTRY(PFS_FSTAB_NODE);

struct fs_mount_t *prv_mount = &FS_FSTAB_ENTRY(PFS_FSTAB_NODE);

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

int pfs_init(void) {
  if (prv_inst.initialized == true) {
    return -EALREADY;
  }

  LOG_INF("Mounted %s", prv_mount->mnt_point);

  return 0;
}

int pfs_open(pfs_file_t *file, const char *filename, fs_mode_t flags) {
  if (file == NULL || filename == NULL) {
    return -EINVAL;
  }

  int ret = fs_open(file, filename, flags);

  return ret;
}

ssize_t pfs_write(pfs_file_t *file, const void *src, size_t len) {
  if (file == NULL || src == NULL || len == 0) {
    return -EINVAL;
  }

  ssize_t ret = fs_write(file, src, len);

  return ret;
}

ssize_t pfs_read(pfs_file_t *file, void *dst, size_t len) {
  if (file == NULL || dst == NULL || len == 0) {
    return -EINVAL;
  }

  ssize_t ret = fs_read(file, dst, len);

  return ret;
}

int pfs_seek(pfs_file_t *file, off_t offset, int whence) {
  if (file == NULL) {
    return -EINVAL;
  }

  int ret = fs_seek(file, offset, whence);

  return ret;
}

int pfs_truncate(pfs_file_t *file, off_t offset) {
  if (file == NULL) {
    return -EINVAL;
  }

  int ret = fs_truncate(file, offset);

  return ret;
}

int pfs_close(pfs_file_t *file) {
  if (file == NULL) {
    return -EINVAL;
  }

  int ret = fs_close(file);

  return ret;
}

int pfs_sync(pfs_file_t *file) {
  if (file == NULL) {
    return -EINVAL;
  }

  int ret = fs_sync(file);

  return ret;
}

/*****************************************************************************
 * Shell Functions
 *****************************************************************************/

#ifdef CONFIG_SHELL

static pfs_file_t prv_shell_file;

static bool prv_shell_file_open = false;

static int prv_shell_open_file(const struct shell *sh, size_t argc,
                               char **argv) {
  fs_mode_t flags = 0;

  if (prv_shell_file_open == true) {
    shell_warn(sh, "A file is already open.");
    return -EALREADY;
  }

  char *filename = argv[1];
  char *in_flags = argv[2];

  for (size_t i = 0; i < strlen(in_flags); i++) {
    switch (in_flags[i]) {
    case 'r':
    case 'R':
      flags |= FS_O_READ;
      break;

    case 'w':
    case 'W':
      flags |= FS_O_WRITE;
      break;

    case 'c':
    case 'C':
      flags |= FS_O_CREATE;
      break;

    default:
      break;
    }
  }

  int ret = pfs_open(&prv_shell_file, filename, flags);

  if (ret < 0) {
    shell_error(sh, "Failed to open %s: %d", filename, ret);
    return ret;
  }

  prv_shell_file_open = true;

  return 0;
}

static int prv_shell_close_file(const struct shell *sh, size_t argc,
                                char **argv) {
  if (prv_shell_file_open == false) {
    shell_warn(sh, "No file open");
    return -ENFILE;
  }

  int ret = pfs_close(&prv_shell_file);
  if (ret < 0) {
    shell_error(sh, "Failed to close file: %d", ret);
  }

  // Assume that if a file can't be closed, then it's probably not open.  At
  // least in this context
  prv_shell_file_open = false;

  return ret;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_pfs, SHELL_CMD_ARG(open, NULL, "Open file.", prv_shell_open_file, 3, 0),
    SHELL_CMD(close, NULL, "Close open file.", prv_shell_close_file),
    SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(pfs, &sub_pfs, "Pebble Filesystem Commands", NULL);
#endif
