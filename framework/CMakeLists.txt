
# Recursively find all header files & add all the directories containing those headers to the include paths
file(GLOB_RECURSE HEADER_FILES "headers/*.h*")
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
    list(APPEND HEADER_DIRS ${HEADER_DIR})
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS)


list(APPEND HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../external_deps)
list(APPEND HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../external_deps/ThorsSerializer)

set(FRAMEWORK_LIB_NAME "framework")

# Create library of source files
file(GLOB_RECURSE SOURCES "sources/*.cpp")
add_library(${FRAMEWORK_LIB_NAME} SHARED ${SOURCES})
target_include_directories(${FRAMEWORK_LIB_NAME} PUBLIC 
    "$<BUILD_INTERFACE:${HEADER_DIRS}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/${FRAMEWORK_LIB_NAME}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/${FRAMEWORK_LIB_NAME}/ThorsSerializer>"
)
set_target_properties(${FRAMEWORK_LIB_NAME} PROPERTIES 
    PUBLIC_HEADER "${HEADER_FILES}"
)

include(${PROJECT_SOURCE_DIR}/functions.cmake)
install_and_create_config(${FRAMEWORK_LIB_NAME})

# Loop over every main file and create executable
file(GLOB MAINS "mains/*.cpp")
foreach(MAIN ${MAINS})
    get_filename_component(FILENAME ${MAIN} NAME_WE)
    add_executable(${FILENAME} ${MAIN})
    target_link_libraries(${FILENAME} PUBLIC ${FRAMEWORK_LIB_NAME} pq)
    install(TARGETS ${FILENAME} RUNTIME)
endforeach()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates DESTINATION /var/lib/blnk)
