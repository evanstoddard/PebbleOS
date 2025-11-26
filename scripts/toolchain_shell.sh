#!/bin/bash

# Paths/Directories
g_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Prevents this script from being run directly
if [ ${0##*/} == ${BASH_SOURCE[0]##*/} ]; then 
    bash --rcfile <(echo ". \"$g_SCRIPT_DIR/toolchain_shell.sh\"; exec bash -i")
    exit 0
fi

# Source common_config.sh for common variables
source ${g_SCRIPT_DIR}/common.sh
