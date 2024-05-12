#!/bin/bash

# Use find to search for the Sandbox folder in the home directory
SANDBOX_PATH=$(find ~ -type d -name "GO-sandbox" -print -quit)

if [ -n "$SANDBOX_PATH" ]; then
    echo "GO Sandbox folder found at: $SANDBOX_PATH"
    cd $SANDBOX_PATH
    # git pull origin prod
    make build_closure
    cd -

    mkdir -p shared

    for so_file in $SANDBOX_PATH/shared/*.so; do
        # Extract the base name (without extension)
        base_name=$(basename "$so_file" .so)
        
        # Check if a .h file with the same name exists
        if [ -e $SANDBOX_PATH/shared/"${base_name}.h" ]; then
            # If .h file exists, copy the .so file
            cp "$so_file" "./shared/${base_name}.so"
            echo "Copied ${so_file}"
        fi
    done
    
else
    echo "GO Sandbox folder not found."
fi