function(install_and_create_config_header_only LIB_NAME DIRECTORIES_TO_INSTALL FILES_TO_INSTALL)
    message(STATUS "Installing and validating external dependency: ${LIB_NAME}")
    
    set(INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/blnk/${LIB_NAME})
    # Install directories
    foreach(DIR ${DIRECTORIES_TO_INSTALL})
        message(STATUS "Installing directory: ${DIR}")
        install(DIRECTORY ${DIR}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blnk/${LIB_NAME}
        )
        get_filename_component(MY_DIR ${DIR} NAME)
        list(APPEND DIR_NAMES ${MY_DIR})
    endforeach()

    # Install files
    foreach(FILE ${FILES_TO_INSTALL})
        message(STATUS "Installing file: ${FILE}")
        install(FILES ${FILE}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blnk/${LIB_NAME}
        )
    endforeach()

    # Validation logic will be included in the generated package config
    message(STATUS "Validation of installed target for: ${LIB_NAME} will be handled by package configuration.")

    # Set paths and target export file for the package
    set(CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake")
    set(CONFIG_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake")
    
    include(CMakePackageConfigHelpers)

    # Write package version file
    write_basic_package_version_file(
        ${CONFIG_VERSION_FILE}
        VERSION 1.0.0
        COMPATIBILITY AnyNewerVersion
    )

    # Configure the package config file
    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/configNoLib.cmake.in
        ${CONFIG_FILE}
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
        PATH_VARS INCLUDE_DIR DIR_NAMES LIB_NAME
    )

    # Install the generated files to make them discoverable
    install(FILES ${CONFIG_FILE} ${CONFIG_VERSION_FILE}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
    )

endfunction()


function(install_and_create_config LIB_NAME)
    install(CODE "file(REMOVE_RECURSE \"${CMAKE_INSTALL_INCLUDEDIR}/blnk/${LIB_NAME}\")")


    install(TARGETS ${LIB_NAME}
        EXPORT ${LIB_NAME}Targets
        ARCHIVE
        PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blnk/${LIB_NAME}
    )
    #setting variables that will be used in Config.cmake.in
    # set(LIB_NAME "${LIB_NAME}")
    set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
    set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/blnk/${LIB_NAME})
    set(TARGET_EXPORT_FILE "${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}/${LIB_NAME}Targets.cmake")

    install (EXPORT ${LIB_NAME}Targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
    )


    # Create the Config file for find_package to work
    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake"
        VERSION 1.0.0
        COMPATIBILITY AnyNewerVersion
    )

    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
        PATH_VARS LIB_INSTALL_DIR INCLUDE_INSTALL_DIR TARGET_EXPORT_FILE LIB_NAME
    )

    # Install the config and version files
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME})
endfunction()