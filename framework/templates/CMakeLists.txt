cmake_minimum_required(VERSION 3.22)

project(factory_%namespace%)

set(CMAKE_CXX_STANDARD 17)

# Find PostgreSQL
find_package(PostgreSQL REQUIRED)
if(PostgreSQL_FOUND)
    include_directories(${PostgreSQL_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "PostgreSQL not found. Install PostgreSQL development libraries.")
endif()

# Find framework


# Recursively find all header files & add all the directories containing those headers to the include paths
file(GLOB_RECURSE HEADER_FILES "headers/*.h*")
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
    list(APPEND HEADER_DIRS ${HEADER_DIR})
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS)

list(APPEND HEADER_DIRS /usr/local/include/blnk/framework)
list(APPEND HEADER_DIRS /usr/local/include/blnk/exdeps)
list(APPEND HEADER_DIRS /usr/local/include/blnk/exdeps/ThorsSerializer)


# Create library of source files
file(GLOB SOURCES "sources/*.cpp")
add_library(factory%namespace% SHARED ${SOURCES})
target_compile_definitions(factory%namespace% PRIVATE SKIP_ENTRY_POINT)
target_link_libraries(factory%namespace% PUBLIC framework pq)
target_include_directories(factory%namespace% PUBLIC
    "$<BUILD_INTERFACE:${HEADER_DIRS}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/factory%namespace%>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/exdeps>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/exdeps/ThorsSerializer>"
)
set_target_properties(factory%namespace% PROPERTIES 
    PUBLIC_HEADER "${HEADER_FILES}"
)

include(GNUInstallDirs)
install(CODE "file(REMOVE_RECURSE \"${CMAKE_INSTALL_INCLUDEDIR}/blnk/factory%namespace%\")")
install(TARGETS factory%namespace%
    ARCHIVE
    PUBLIC_HEADER
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blnk/factory%namespace%
)
if (UNIX)
    install(CODE "execute_process(COMMAND ldconfig)")
endif()
