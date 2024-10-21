cmake_minimum_required(VERSION 3.22)

project(cpp_framework)

set(CMAKE_CXX_STANDARD 17)

# Find PostgreSQL
find_package(PostgreSQL REQUIRED)
if(PostgreSQL_FOUND)
    include_directories(${PostgreSQL_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "PostgreSQL not found. Install PostgreSQL development libraries.")
endif()


add_subdirectory(framework)
add_subdirectory(Webserver-Backend)

# set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# set(CMAKE_INSTALL_RPATH "/usr/local/lib")

include(GNUInstallDirs)
install(CODE "file(REMOVE_RECURSE \"${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework\")")
install(TARGETS framework
    ARCHIVE
    PUBLIC_HEADER
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework
)
if (UNIX)
    install(CODE "execute_process(COMMAND ldconfig)")
endif()