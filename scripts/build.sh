#!/bin/bash

# Paths/Directories
g_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Source common.sh for common variables
source ${g_SCRIPT_DIR}/common.sh

# Ensure project initialized
if [ ! -d ${g_ROOT_DIR}/.west ]; then
    echo "Project doesn't exist..."
    ${g_SCRIPT_DIR}/init_project.sh
fi

# Build default target with default board
west build --board=${g_DEFAULT_BUILD_BOARD} ${g_PROJECT_DIR}/${g_DEFAULT_BUILD_TARGET} $@
