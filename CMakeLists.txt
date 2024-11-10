cmake_minimum_required(VERSION 3.22)

project(cpp_framework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

include(GNUInstallDirs)

# Find dependencies
find_package(PostgreSQL)
if(PostgreSQL_FOUND)
    include_directories(${PostgreSQL_INCLUDE_DIRS})
    link_directories(${PostgreSQL_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "PostgreSQL not found. Install PostgreSQL development libraries.")
endif()

find_package(Threads)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "Threads not found. Install pthreads development libraries.")
endif()

# Library names
set(EXTERNAL_DEPS_LIB_NAME "blnkExternalDeps")
set(FRAMEWORK_LIB_NAME "blnkFramework")
set(WEBSERVER_LIB_NAME "blnkWebserver")
set(MIDDLEWARE_PREFIX_LIB_NAME "blnk")


add_subdirectory(external_deps)
add_subdirectory(framework)
add_subdirectory(Webserver-Backend)

# Check middleware dependencies before compiling middlewares
find_package(Poco COMPONENTS Foundation JWT Crypto JSON)
if (Poco_FOUND)
    message(STATUS "Found POCO Foundation & JWT. Compiling middlewares.")
    add_subdirectory(middlewares)
else()
    message(WARNING "POCO Foundation & JWT not found. Skipping middlewares.")
endif()


if (UNIX)
    install(CODE "execute_process(COMMAND ldconfig)")
endif()