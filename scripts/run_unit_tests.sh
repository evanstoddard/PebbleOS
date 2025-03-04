#!/bin/bash

# Get path to scripts directory
g_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Source common shell script
source ${g_SCRIPT_DIR}/common.sh

# Bail early on error
set -e

function run_tests() {
    pushd ${g_PEBBLE_OS_DIR}
    west twister -p native_sim/native/64 -T . -O ${g_ROOT_DIR}/build_tests $@
    popd
}

function install_deps() {
    pip3 install -r ${g_ROOT_DIR}/zephyr/scripts/requirements.txt
}

function main() {
    activate_virtual_env
    install_deps

    ${g_SCRIPT_DIR}/init_project.sh

    run_tests $@
}

main $@