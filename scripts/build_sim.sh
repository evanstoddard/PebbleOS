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
pushd ${g_ROOT_DIR}
west build --board=native_sim/native/64 ${g_PROJECT_DIR}/${g_DEFAULT_BUILD_TARGET} -d build_sim $@
popd
