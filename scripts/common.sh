g_ROOT_DIR=${g_SCRIPT_DIR}/..
g_VENV=${g_ROOT_DIR}/.venv

g_PEBBLE_OS_DIR=${g_ROOT_DIR}/PebbleOS

# Will activate virtual environment if it exists
function activate_virtual_env() {
    
    # If environment doesn't exist, create environment and install west
    if [ ! -f ${g_VENV}/bin/activate ]; then
        create_virtual_env
        
        source ${g_VENV}/bin/activate

        echo "Installing west."
        pip3 install west pyelftools
        return 0
    fi

    echo "Activating virtual environment."
    source ${g_VENV}/bin/activate
}

# Will create virtual environment
function create_virtual_env() {
    echo "Virtual environment doesn't exist... creating..."
    python3 -m venv ${g_VENV}
}