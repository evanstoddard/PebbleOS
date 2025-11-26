#!/bin/bash

# Prevents this script from being run directly
if [ ${0##*/} == ${BASH_SOURCE[0]##*/} ]; then 
    echo "WARNING"
    echo "This script is not meant to be executed directly!"
    echo "Use this script only by sourcing it."
    echo
    exit 1
fi

# Brief: Common variables and functions used by scripts in this directory

# Source project config
source ${g_SCRIPT_DIR}/project.conf

# Paths/Directories
g_ROOT_DIR=${g_SCRIPT_DIR}/..
g_PROJECT_DIR=${g_ROOT_DIR}/${g_PROJECT_NAME}
g_VENV_PATH=${g_ROOT_DIR}/.venv

function comm_initialize_venv() {
    python -m venv ${g_VENV_PATH}

    source ${g_VENV_PATH}/bin/activate

    pip install -r ${g_SCRIPT_DIR}/requirements.txt
}

function comm_setup_venv() {
    if [ ! -f ${g_VENV_PATH}/bin/activate ]; then
        comm_initialize_venv
    fi

    source ${g_VENV_PATH}/bin/activate

    which west &> /dev/null

    if [ $? != 0 ]; then
        comm_initialize_venv
    fi;
}

comm_setup_venv
