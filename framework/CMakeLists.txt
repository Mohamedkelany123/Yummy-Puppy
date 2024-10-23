
# Recursively find all header files & add all the directories containing those headers to the include paths
file(GLOB_RECURSE HEADER_FILES "headers/*.h*")
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
    list(APPEND HEADER_DIRS ${HEADER_DIR})
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS)


list(APPEND HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../external_deps)
list(APPEND HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../external_deps/ThorsSerializer)

# Create library of source files
file(GLOB_RECURSE SOURCES "sources/*.cpp")
add_library(framework SHARED ${SOURCES})
target_include_directories(framework PUBLIC 
    "$<BUILD_INTERFACE:${HEADER_DIRS}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework/ThorsSerializer>"
)
set_target_properties(framework PROPERTIES 
    PUBLIC_HEADER "${HEADER_FILES}"
)

install(CODE "file(REMOVE_RECURSE \"${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework\")")


install(TARGETS framework
    EXPORT frameworkTargets
    ARCHIVE
    PUBLIC_HEADER
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework
)

install (EXPORT frameworkTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/framework
)

#setting variables that will be used in Config.cmake.in
set(LIB_NAME "framework")
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework)
set(TARGET_EXPORT_FILE "${CMAKE_INSTALL_LIBDIR}/cmake/framework/frameworkTargets.cmake")

# Create the Config file for find_package to work
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/frameworkConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworkConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/frameworkConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/framework
    PATH_VARS LIB_INSTALL_DIR INCLUDE_INSTALL_DIR TARGET_EXPORT_FILE
)

# Install the config and version files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/frameworkConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/frameworkConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/framework)


# Loop over every main file and create executable
file(GLOB MAINS "mains/*.cpp")
foreach(MAIN ${MAINS})
    get_filename_component(FILENAME ${MAIN} NAME_WE)
    add_executable(${FILENAME} ${MAIN})
    target_link_libraries(${FILENAME} PUBLIC framework pq)
    install(TARGETS ${FILENAME} RUNTIME)
endforeach()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates DESTINATION /var/lib/blnk)
