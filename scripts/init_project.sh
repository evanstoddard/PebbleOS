#!/bin/bash

# Get path to scripts directory
g_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Source common shell script
source ${g_SCRIPT_DIR}/common.sh

# Bail early on error
set -e

function initialize_workspace() {
    # Don't initialize workspace if already initialized
    if [ -f ${g_ROOT_DIR}/.west/config ]; then
        return 0;
    fi

    west init -l ${g_PEBBLE_OS_DIR}
}

function update_workspace() {
    west update
}

function main() {
    activate_virtual_env
    initialize_workspace
    update_workspace
}

main
