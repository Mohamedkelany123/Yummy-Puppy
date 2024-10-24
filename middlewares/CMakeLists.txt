

# Recursively find all header files & add all the directories containing those headers to the include paths
file(GLOB_RECURSE HEADER_FILES "headers/*.h*")
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
    list(APPEND HEADER_DIRS ${HEADER_DIR})
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS)

include(${PROJECT_SOURCE_DIR}/functions.cmake)


# Loop over every main file and create executable
file(GLOB MIDDLEWARES "sources/*.cpp")
foreach(MIDDLEWARE ${MIDDLEWARES})
    get_filename_component(FILENAME ${MIDDLEWARE} NAME_WE)
    add_library(${FILENAME} SHARED ${MIDDLEWARE})
    target_link_libraries(${FILENAME} PRIVATE webserver)
    target_include_directories(${FILENAME} PUBLIC 
        "$<BUILD_INTERFACE:${HEADER_DIRS}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/webserver>"
    )
    set_target_properties(${FILENAME} PROPERTIES 
        PUBLIC_HEADER "${HEADER_FILES}"
    )
    install_and_create_config(${FILENAME})

endforeach()