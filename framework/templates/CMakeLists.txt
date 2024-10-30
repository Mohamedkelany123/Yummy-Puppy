cmake_minimum_required(VERSION 3.22)

project(factory_%namespace%)

set(CMAKE_CXX_STANDARD 17)

set(FACTORY_LIB_NAME "blnkFactory%namespace%")

include(GNUInstallDirs)
include(${PROJECT_SOURCE_DIR}/functions.cmake)

set(LIBRARY_DEPS "blnkFramework")


# Find Packages
find_package(PostgreSQL)
if(PostgreSQL_FOUND)
    include_directories(${PostgreSQL_INCLUDE_DIRS})
    link_directories(${PostgreSQL_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "PostgreSQL not found. Install PostgreSQL development libraries.")
endif()

set(PACKAGES_DEPS "blnkExternalDeps" ${LIBRARY_DEPS})
foreach(DEPS ${PACKAGES_DEPS})
    find_and_include_package(${DEPS})
endforeach()

# Recursively find all header files & add all the directories containing those headers to the include paths
file(GLOB_RECURSE HEADER_FILES "headers/*.h*")
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
    list(APPEND HEADER_DIRS ${HEADER_DIR})
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS)


# Create library of source files
file(GLOB SOURCES "sources/*.cpp")
add_library(${FACTORY_LIB_NAME} SHARED ${SOURCES})
target_compile_definitions(${FACTORY_LIB_NAME} PRIVATE SKIP_ENTRY_POINT)
target_link_libraries(${FACTORY_LIB_NAME} PUBLIC ${LIBRARY_DEPS} pq)
target_include_directories(${FACTORY_LIB_NAME} PUBLIC
    "$<BUILD_INTERFACE:${HEADER_DIRS}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/${FACTORY_LIB_NAME}>"
)
set_target_properties(${FACTORY_LIB_NAME} PROPERTIES 
    PUBLIC_HEADER "${HEADER_FILES}"
)

install_and_create_config(${FACTORY_LIB_NAME})

if (UNIX)
    install(CODE "execute_process(COMMAND ldconfig)")
endif()
