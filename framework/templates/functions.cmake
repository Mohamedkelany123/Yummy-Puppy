function(find_and_include_package PACKAGE_NAME)
    find_package(${PACKAGE_NAME} REQUIRED)
    if(${PACKAGE_NAME}_FOUND)
        foreach(INCLUDE_DIR ${${PACKAGE_NAME}_INCLUDE_DIRS})
            message(STATUS "${PACKAGE_NAME} INCLUDES = ${INCLUDE_DIR}")
            include_directories(${INCLUDE_DIR})    
        endforeach()
        
    else()
        message(FATAL_ERROR "${PACKAGE_NAME} not found. Install ${PACKAGE_NAME} development libraries.")
    endif()
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