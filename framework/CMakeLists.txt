set(TEMPLATE_INSTALL_DIR /var/lib/blnk)

include(${PROJECT_SOURCE_DIR}/functions.cmake)
get_header_files_and_dirs("${CMAKE_CURRENT_SOURCE_DIR}/headers" HEADER_DIRS HEADER_FILES)

list(APPEND HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../external_deps)
list(APPEND HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../external_deps/ThorsSerializer)


# Create library of source files
file(GLOB_RECURSE SOURCES "sources/*.cpp")
add_library(${FRAMEWORK_LIB_NAME} SHARED ${SOURCES})
target_include_directories(${FRAMEWORK_LIB_NAME} PUBLIC 
    "$<BUILD_INTERFACE:${HEADER_DIRS}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/${FRAMEWORK_LIB_NAME}>"
)
target_link_libraries(${FRAMEWORK_LIB_NAME} PUBLIC pq)
set_target_properties(${FRAMEWORK_LIB_NAME} PROPERTIES 
    PUBLIC_HEADER "${HEADER_FILES}"
)

#compiling as a system library and creating necessary config file to be able to find package from any external project
install_and_create_config(${FRAMEWORK_LIB_NAME})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates DESTINATION ${TEMPLATE_INSTALL_DIR})

# Loop over every main file and create executable
file(GLOB MAINS "mains/*.cpp")
foreach(MAIN ${MAINS})
    get_filename_component(FILENAME ${MAIN} NAME_WE)
    add_executable(${FILENAME} ${MAIN})
    target_link_libraries(${FILENAME} PUBLIC ${FRAMEWORK_LIB_NAME} pq)
    install(TARGETS ${FILENAME} RUNTIME)
endforeach()
