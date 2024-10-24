# Recursively find all header files & add all the directories containing those headers to the include paths
file(GLOB_RECURSE HEADER_FILES "headers/*.h*")

foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
    list(APPEND HEADER_DIRS ${HEADER_DIR})
endforeach()

list(REMOVE_DUPLICATES HEADER_DIRS)

set(WEBSERVER_LIB_NAME "webserver")

# Create library of source files
file(GLOB_RECURSE SOURCES "sources/*.cpp")
add_library(${WEBSERVER_LIB_NAME} SHARED ${SOURCES})

target_link_libraries(${WEBSERVER_LIB_NAME} PUBLIC framework pq)

target_include_directories(${WEBSERVER_LIB_NAME} PUBLIC
    "$<BUILD_INTERFACE:${HEADER_DIRS}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(${WEBSERVER_LIB_NAME} PROPERTIES
    PUBLIC_HEADER "${HEADER_FILES}"
)

set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "/usr/local/lib")

include(${PROJECT_SOURCE_DIR}/functions.cmake)
install_and_create_config(${WEBSERVER_LIB_NAME})

# Loop over every main file and create executable
file(GLOB MAINS "mains/*.cpp")

foreach(MAIN ${MAINS})
    get_filename_component(FILENAME ${MAIN} NAME_WE)
    add_executable(${FILENAME} ${MAIN})
    target_link_libraries(${FILENAME} PRIVATE ${WEBSERVER_LIB_NAME})
    install(TARGETS ${FILENAME} RUNTIME)
endforeach()