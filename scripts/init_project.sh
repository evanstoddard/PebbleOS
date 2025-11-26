#!/bin/bash

# Paths/Directories
g_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Source common.sh for common variables
source ${g_SCRIPT_DIR}/common.sh

# Initialize project
function initialize_project() {
    if [ ! -d ${g_ROOT_DIR}/.west ]; then
        echo "Initializing project..."
        west init -l ${g_PROJECT_DIR}
    fi

    pip install -r ${g_ROOT_DIR}/zephyr/scripts/requirements.txt
}

function run_west_update() {
    # Change into repo root
    pushd ${g_ROOT_DIR}
    
    # Run the following west update command
    west update -f smart -n -o=--depth=1
    
    popd
}

initialize_project
run_west_update
