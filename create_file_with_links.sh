#!/bin/bash

# Root directory of your project
PROJECT_ROOT="$(pwd)"

# Function to create a folder that will contain subfolders
create_folder() {
    local target_dir="$1"  # Target directory where the new folder will be created
    local folder_name="$2" # Name of the folder to be created

    # Calculate the relative path to the project root directory before creating the new folder
    relative_path=$(realpath --relative-to="$target_dir/$folder_name" "$PROJECT_ROOT")

    # Create the target directory if it doesn't exist
    mkdir -p "$target_dir/$folder_name"

    echo "$relative_path/recursive_makefile" 

    # Navigate to the target directory
    cd "$target_dir/$folder_name"

    # Create symbolic links to the recursive_makefile
    ln -s "$relative_path/recursive_makefile" Makefile

    echo "Folder '$folder_name' created with symbolic links in '$target_dir/$folder_name'"
}

# Function to create normal .cpp files which are not executables.
create_lib() {
    local target_dir="$1"  # Target directory where the new folder will be created
    local lib_name="$2"    # Name of the library folder to be created

    # Calculate the relative path to the project root directory before creating the new folder
    relative_path=$(realpath --relative-to="$target_dir/$lib_name" "$PROJECT_ROOT")

    # Create the target directory if it doesn't exist
    mkdir -p "$target_dir/$lib_name"

    # Navigate to the target directory
    cd "$target_dir/$lib_name"

    # Create symbolic links to the leaf_makefile and leaf_makefile.vars
    ln -s "$relative_path/leaf_makefile.vars" leaf_makefile.vars
    ln -s "$relative_path/leaf_makefile" Makefile

    echo "Library folder '$lib_name' created with symbolic links in '$target_dir/$lib_name'"
}


# Function to create directory that will contain .cpp files that will then be executables
create_exec() {
    local target_dir="$1"  # Target directory where the new folder will be created
    local exec_name="$2"   # Name of the exec folder to be created

    # Calculate the relative path to the project root directory before creating the new folder
    # Add one more level of `../` to account for the newly created directory
    relative_path=$(realpath --relative-to="$target_dir/$exec_name" "$PROJECT_ROOT")

    # Create the target directory if it doesn't exist
    mkdir -p "$target_dir/$exec_name"

    # Navigate to the target directory
    cd "$target_dir/$exec_name"

    echo "$relative_path/leaf_bin_makefile"

    # Create symbolic links to the bin_makefile, leaf_makefile.vars, and leaf_bin_makefile
    ln -s "$relative_path/leaf_makefile.vars" leaf_makefile.vars
    ln -s "$relative_path/leaf_bin_makefile" Makefile

    echo "Exec folder '$exec_name' created with symbolic links in '$target_dir/$exec_name'"
}

create_dso() {
    local target_dir="$1"  # Target directory where the new folder will be created
    local exec_name="$2"   # Name of the exec folder to be created

    relative_path=$(realpath --relative-to="$target_dir/$exec_name" "$PROJECT_ROOT")

    # Create the target directory if it doesn't exist
    mkdir -p "$target_dir/$exec_name"

    # Navigate to the target directory
    cd "$target_dir/$exec_name"

    echo "$relative_path/leaf_dso_makefile"

    #Create symbolic links
    ln -s "$relative_path/leaf_makefile.vars" leaf_makefile.vars
    ln -s "$relative_path/leaf_dso_makefile" Makefile

    echo "DSO folder '$exec_name' created with symbolic links in '$target_dir/$exec_name'"
}

# Check if the correct number of arguments is provided
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <type> <target_directory> <folder_name> "
    echo "Type can be 'folder' , 'lib' , 'dso' ,or 'exec"
    exit 1
fi

type="$1"
target_directory="$2"
folder_name="$3"

case "$type" in
    folder)
        create_folder "$target_directory" "$folder_name"
        ;;
    lib)
        create_lib "$target_directory" "$folder_name"
        ;;
    exec)
        create_exec "$target_directory" "$folder_name"
        ;;
    dso)
        create_dso "$target_directory" "$folder_name"
        ;;
    *)
        echo "Invalid type. Type can be 'folder' , 'lib' ,or 'exec"
        exit 1
        ;;
esac


