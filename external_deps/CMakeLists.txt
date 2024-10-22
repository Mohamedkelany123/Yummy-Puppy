
# Recursively get all files and directories
file(GLOB ITEMS_TO_INSTALL "${CMAKE_CURRENT_SOURCE_DIR}/*")

# Loop over each item and check if it's a directory or a file
foreach(ITEM ${ITEMS_TO_INSTALL})

    # Check if the file is CMakeLists.txt and skip it
    get_filename_component(FILENAME ${ITEM} NAME)
    if(FILENAME STREQUAL "CMakeLists.txt")
        continue()  # Skip this file
    endif()

    if(IS_DIRECTORY ${ITEM})
        message(STATUS "Installing directory: ${ITEM}")
        install(DIRECTORY ${ITEM} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework)
    else()
        message(STATUS "Installing file: ${ITEM}")
        install(FILES ${ITEM} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework)
    endif()
endforeach()
