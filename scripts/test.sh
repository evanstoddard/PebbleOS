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

TEST_DIR=${1:-${g_PROJECT_DIR}/tests/os/events}

pushd ${g_ROOT_DIR}
west build --board=native_sim/native/64 ${TEST_DIR} -d build_test $@ && \
    ./build_test/zephyr/zephyr.exe
popd
