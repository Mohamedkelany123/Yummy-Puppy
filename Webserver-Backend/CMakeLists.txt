include(${PROJECT_SOURCE_DIR}/functions.cmake)

get_header_files_and_dirs("${CMAKE_CURRENT_SOURCE_DIR}/headers" HEADER_DIRS HEADER_FILES)

# Create library of source files
file(GLOB_RECURSE SOURCES "sources/*.cpp")
add_library(${WEBSERVER_LIB_NAME} SHARED ${SOURCES})

target_link_libraries(${WEBSERVER_LIB_NAME} PUBLIC ${FRAMEWORK_LIB_NAME} pq)

target_include_directories(${WEBSERVER_LIB_NAME} PUBLIC
    "$<BUILD_INTERFACE:${HEADER_DIRS}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(${WEBSERVER_LIB_NAME} PROPERTIES
    PUBLIC_HEADER "${HEADER_FILES}"
)

# TODO: investigate further
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "/usr/local/lib")

install_and_create_config(${WEBSERVER_LIB_NAME})

# Loop over every main file and create executable
file(GLOB MAINS "mains/*.cpp")
foreach(MAIN ${MAINS})
    get_filename_component(FILENAME ${MAIN} NAME_WE)
    add_executable(${FILENAME} ${MAIN})
    target_link_libraries(${FILENAME} PRIVATE ${WEBSERVER_LIB_NAME})
    install(TARGETS ${FILENAME} RUNTIME)
endforeach()