include(${PROJECT_SOURCE_DIR}/functions.cmake)

get_header_files_and_dirs("${CMAKE_CURRENT_SOURCE_DIR}/headers" HEADER_DIRS HEADER_FILES)

# Loop over every main file and create executable
file(GLOB MIDDLEWARES "sources/*.cpp")
foreach(MIDDLEWARE ${MIDDLEWARES})
    get_filename_component(FILENAME ${MIDDLEWARE} NAME_WE)
    set(LIB_NAME ${MIDDLEWARE_PREFIX_LIB_NAME}${FILENAME})
    add_library(${LIB_NAME} SHARED ${MIDDLEWARE})
    target_link_libraries(${LIB_NAME} PRIVATE ${WEBSERVER_LIB_NAME})
    target_include_directories(${LIB_NAME} PUBLIC 
        "$<BUILD_INTERFACE:${HEADER_DIRS}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/blnk/${WEBSERVER_LIB_NAME}>"
    )
    set_target_properties(${LIB_NAME} PROPERTIES 
        PUBLIC_HEADER "${HEADER_FILES}"
    )
    install_and_create_config(${LIB_NAME})
endforeach()