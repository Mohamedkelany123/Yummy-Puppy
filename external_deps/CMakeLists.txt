# Recursively get all files and directories
file(GLOB ITEMS_TO_INSTALL "${CMAKE_CURRENT_SOURCE_DIR}/*")

set(EXTERNAL_DEPS_LIB_NAME "exdeps")

# Define variables to hold directories and files to be installed
set(DIRECTORIES_TO_INSTALL "")
set(FILES_TO_INSTALL "")

# Loop over each item and add to the appropriate variable
foreach(ITEM ${ITEMS_TO_INSTALL})
    # Check if the file is CMakeLists.txt and skip it
    get_filename_component(FILENAME ${ITEM} NAME)
    if(FILENAME STREQUAL "CMakeLists.txt")
        continue()  # Skip this file
    endif()

    if(IS_DIRECTORY ${ITEM})
        message(STATUS "Adding directory to install list: ${ITEM}")
        list(APPEND DIRECTORIES_TO_INSTALL ${ITEM})
    else()
        message(STATUS "Adding file to install list: ${ITEM}")
        list(APPEND FILES_TO_INSTALL ${ITEM})
    endif()
endforeach()

# Include functions and install the directories and files
include(${PROJECT_SOURCE_DIR}/functions.cmake)
install_and_create_config_header_only(${EXTERNAL_DEPS_LIB_NAME} "${DIRECTORIES_TO_INSTALL}" "${FILES_TO_INSTALL}")