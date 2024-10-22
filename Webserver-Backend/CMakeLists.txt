

# Recursively find all header files & add all the directories containing those headers to the include paths
file(GLOB_RECURSE HEADER_FILES "headers/*.h*")
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
    list(APPEND HEADER_DIRS ${HEADER_DIR})
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS)

# Create library of source files
file(GLOB_RECURSE SOURCES "sources/*.cpp")
add_library(webserver ${SOURCES})
target_link_libraries(webserver PUBLIC framework pq)
target_include_directories(webserver PUBLIC 
    "$<BUILD_INTERFACE:${HEADER_DIRS}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(webserver PROPERTIES 
    PUBLIC_HEADER "${HEADER_FILES}"
)

# Loop over every main file and create executable
file(GLOB MAINS "mains/*.cpp")
foreach(MAIN ${MAINS})
    get_filename_component(FILENAME ${MAIN} NAME_WE)
    add_executable(${FILENAME} ${MAIN})
    target_link_libraries(${FILENAME} PRIVATE webserver)
    install(TARGETS ${FILENAME} RUNTIME)
endforeach()