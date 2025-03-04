#!/bin/bash

# Get path to scripts directory
g_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Source common shell script
source ${g_SCRIPT_DIR}/common.sh

# Bail early on error
set -e

function build() {
    pushd ${g_PEBBLE_OS_DIR}
    west build -b native_sim/native/64 CoreOS -d ${g_ROOT_DIR}/build_sim $@
    popd
}

function main() {
    activate_virtual_env
    ${g_SCRIPT_DIR}/init_project.sh
    build $@
}

main $@