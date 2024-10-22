cmake_minimum_required(VERSION 3.22)

project(cpp_framework)

set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)


# Find PostgreSQL
find_package(PostgreSQL REQUIRED)
if(PostgreSQL_FOUND)
    include_directories(${PostgreSQL_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "PostgreSQL not found. Install PostgreSQL development libraries.")
endif()


add_subdirectory(external_deps)
add_subdirectory(framework)
add_subdirectory(Webserver-Backend)
add_subdirectory(middlewares)


install(CODE "file(REMOVE_RECURSE \"${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework\")")
install(TARGETS framework
ARCHIVE
PUBLIC_HEADER
DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blnk/framework
)

set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "/usr/local/lib")
install(CODE "file(REMOVE_RECURSE \"${CMAKE_INSTALL_INCLUDEDIR}/blnk/webserver\")")
install(TARGETS webserver
    ARCHIVE
    PUBLIC_HEADER
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blnk/webserver
)

if (UNIX)
    install(CODE "execute_process(COMMAND ldconfig)")
endif()