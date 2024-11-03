## CMake Guidelines

### General

- CMake file should be named: `CMakeLists.txt`
- The main CMakeLists file should contain `cmake_minimum_required`, `project`, and `set` for any global compilation configuration like `CMAKE_CXX_STANDARD`.
- To get CMake variables: [CMake documentation](https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html)
- To get CMake modules: [CMake documentation](https://cmake.org/cmake/help/latest/manual/cmake-modules.7.html)

### Applications

- Each application has different dependencies that must be installed before compiling. CMake should specify if compilation will fail when dependencies are not met.
- Each application has different folder structure so each application has different CMake file.
- Each application should be compiled using same commands.
- Application CMake file can be constructed using blocks mentioned below.

### Framework

- The main CMakeLists file includes 4 subdirectories: `framework`, `Webserver-backend`, `middlewares`, and `external_deps` where each subdirectory contains a seperate CMakeLists file.
- `framework` and `Webserver-backend` subdirectories follow same folder structure. The `sources` directory contains .cpp files to be compiled into a single shared object and installed in system lib directory. The `headers` directory contains .h files to be installed in system include directory. The `mains` directory contains .cpp main files where each file is compiled into an executable and installed in system bin directory. Adding files into any of these directories are automatically detected during compilation without needing to change CMake file.
- `middlewares` subdirectory contains `sources` and `headers` directory. The `sources` directory contains .cpp files where each file is compiled into a shared object and installed in system lib directory. The `headers` directory contains .h files to be installed in system include directory. Adding files into any of these directories are automatically detected during compilation without needing to change CMake file.
- `external_deps` subdirectory contains header-only libraries. Each file or folder inside is installed in system include directory. Directories are installed with same internal structure.
- When installing package to system file, `config.cmake.in` file is used for config file generation to be able to `find_package` in another CMake project.
- ORM generator templates are installed to system files in `/var/lib/blnk/`
- All libraries installed have prefix `libblnk` and located in `/usr/local/lib` on Linux machines
- All header files installed are located in `/usr/local/include/blnk/` on Linux machines
- All executable files installed are located in `/usr/local/bin` on Linux machines

## Blocks of common Application CMake use-cases

- ### Required commands for root CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.22) # Required
project(<project_name>) # Required
set(CMAKE_CXX_STANDARD 17)
```

- ### Setting important variables

  - `APP_LIB_NAME`: specifies name of main application library if application contains sources to be compiled into a library.
  - `FACTORY_NAMESPACE`: namespace of factory being used in application
  - `LIBRARY_DEPS`: dependencies to be linked with application code
  - `PACKAGE_DEPS`: dependencies to check if they're installed in system and to include their header directories

```cmake
set(APP_LIB_NAME "los")
set(FACTORY_NAMESPACE "ostaz")
set(LIBRARY_DEPS "blnkFramework" "blnkWebserver" "blnkFactory${FACTORY_NAMESPACE}")
set(PACKAGES_DEPS "PostgreSQL" "blnkExternalDeps" ${LIBRARY_DEPS})
```

- ### Finding and including packages

  - loops over the application's `PACKAGE_DEPS` and checks if they are installed in system. If found, loop over package header directories and include them globally in project.

```cmake
foreach(PACKAGE_NAME ${PACKAGES_DEPS})
    find_package(${PACKAGE_NAME})
    if(${PACKAGE_NAME}_FOUND)
        foreach(INCLUDE_DIR ${${PACKAGE_NAME}_INCLUDE_DIRS})
            message(STATUS "Found ${PACKAGE_NAME} includes: ${INCLUDE_DIR}")
            include_directories(${INCLUDE_DIR})  
        endforeach()
    else()
        message(FATAL_ERROR "${PACKAGE_NAME} not found. Install ${PACKAGE_NAME} development libraries.")
    endif()
endforeach()
```

- ### Finding all header files & directories of application

  - getting all header files (.h) recursively relative to CMake file and appending their directories into list
  - remove duplicate directories from list
  - if header files are in different directory, then path in `file` command should be changed

```cmake
file(GLOB_RECURSE HEADER_FILES "*.h*")
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
    list(APPEND HEADER_DIRS ${HEADER_DIR})
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS)
```

- ### Creating a static library (.a)

  - getting all source files (.cpp) recursively relative to CMake file and creating a static library with name `APP_LIB_NAME`
  - use `target_link_libraries` to link package lib files with library
  - use `target_include_directories` to add include dirs needed by library
  - if source files are in different directory, then path in `file` command should be changed

```cmake
file(GLOB_RECURSE SOURCES "*/sources/*.cpp")
add_library(${APP_LIB_NAME} ${SOURCES})
target_link_libraries(${APP_LIB_NAME} PUBLIC ${LIBRARY_DEPS})
target_compile_options(${APP_LIB_NAME} PRIVATE -fPIC)
target_include_directories(${APP_LIB_NAME} PUBLIC ${HEADER_DIRS})
```

- ### Creating a shared library (.so)

  - getting all source files (.cpp) inside mains directory relative to CMake file and creating a shared object (.so) with source file name for each source file
  - use `set_target_properties` to set prefix of created shared object name to be `""` (default is `lib`)
  - use `target_link_libraries` to link libraries needed by shared object
  - if main source files are in different directory, then path in `file` command should be changed

```cmake
file(GLOB MAINS "mains/*.cpp")
foreach(MAIN ${MAINS})
    get_filename_component(FILENAME ${MAIN} NAME_WE)
    add_library(${FILENAME} SHARED ${MAIN})
    set_target_properties(${FILENAME} PROPERTIES PREFIX "")
    target_compile_options(${FILENAME} PRIVATE -fPIC)
    target_link_libraries(${FILENAME} PRIVATE ${APP_LIB_NAME})
endforeach()
```

- ### Creating executable

  - getting all source files (.cpp) inside mains directory relative to CMake file and creating an executable with source file name for each source file
  - use `target_link_libraries` to link libraries needed by executable
  - if main source files are in different directory, then path in `file` command should be changed

```cmake
file(GLOB MAINS "mains/*.cpp")
foreach(MAIN ${MAINS})
    get_filename_component(FILENAME ${MAIN} NAME_WE)
    add_executable(${FILENAME} ${MAIN})
    target_link_libraries(${FILENAME} PUBLIC ${APP_LIB_NAME})
endforeach()
```

- ### Using functions

  - functions can be used for easier code reusability if project contains several CMake files
  - the file containing the functions is generally named `<name>.cmake`

```cmake
# In CMakeLists.txt
include(${PROJECT_SOURCE_DIR}/functions.cmake)
foreach(DEPS ${PACKAGES_DEPS})
    find_and_include_package(${DEPS})
endforeach()

# In functions.cmake
function(find_and_include_package PACKAGE_NAME)
    find_package(${PACKAGE_NAME})
    if(${PACKAGE_NAME}_FOUND)
        foreach(INCLUDE_DIR ${${PACKAGE_NAME}_INCLUDE_DIRS})
            message(STATUS "Found ${PACKAGE_NAME} includes: ${INCLUDE_DIR}")
            include_directories(${INCLUDE_DIR})  
        endforeach()
      
    else()
        message(FATAL_ERROR "${PACKAGE_NAME} not found. Install ${PACKAGE_NAME} development libraries.")
    endif()
endfunction()
```

- ### Using Subdirectories

  - using subdirectories in CMake can be useful for seperation of concerns, where each subdirectory has its own CMakeLists.txt file containing specific dependency handling and build rules for that part of the project.

```cmake
add_subdirectory(<directory_name>)
```

## Creating system libraries with configuration files

```cmake
#<PLACE_HOLDER>_LIB_NAME is a variable that is set in the begining of the Cmake file or parent file which specifies the name of the library
  
#sinppet form Cmakelists.txt in the root of our project
  # Library names
  set(EXTERNAL_DEPS_LIB_NAME "blnkExternalDeps")
  set(FRAMEWORK_LIB_NAME "blnkFramework")
  set(WEBSERVER_LIB_NAME "blnkWebserver")
  set(MIDDLEWARE_PREFIX_LIB_NAME "blnk")
```

```cmake
#this function encapsulates the logic of installing the system library and creating config file that allow packages to be found from external projects
install_and_create_config(${<PLACE_HOLDER>_LIB_NAME})
```

```cmake
#snippet from functions.cmake

# Function to install a library and create a configuration for find_package to work
function(install_and_create_config LIB_NAME)

# Debugging print to indicate the library being installed
message(STATUS "To install ${LIB_NAME}")

# Step 1: Remove any existing installation of the library's header files
# This ensures that the installation directory is clean before installing the new version
install(CODE "file(REMOVE_RECURSE \"${CMAKE_INSTALL_INCLUDEDIR}/blnk/${LIB_NAME}\")")

# Step 2: Install the target library
# - TARGETS: Specifies the target library to be installed
# - EXPORT: Creates an export set that will be used to create target files for the library
# - ARCHIVE, PUBLIC_HEADER: These arguments specify the type of the files to be installed
# - DESTINATION: Specifies the destination directory for the library installation
install(TARGETS ${LIB_NAME}
    EXPORT  ${LIB_NAME}Targets
    ARCHIVE
    PUBLIC_HEADER
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blnk/${LIB_NAME}
)

# Step 3: Set variables for use in the configuration file
# These variables will be used to fill in the values in the generated Config.cmake.in file
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/blnk/${LIB_NAME})
set(TARGET_EXPORT_FILE "${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}/${LIB_NAME}Targets.cmake")

# Step 4: Install the export target files
# This step installs the export set created in the previous steps to the destination
# The export set contains metadata for the targets that will be used for other projects
install (EXPORT ${LIB_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
)

# Step 5: Create the version file for find_package
# This file helps find_package to check the compatibility of the library version
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

# Step 6: Configure the Config file for find_package to locate the library
# This step uses the template file (config.cmake.in) to generate the final configuration file
# PATH_VARS are the variables that will be replaced in the final Config.cmake file
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
    PATH_VARS LIB_INSTALL_DIR INCLUDE_INSTALL_DIR TARGET_EXPORT_FILE LIB_NAME
)

# Step 7: Install the Config and Version files
# These files are installed to the directory where CMake expects to find them during find_package calls
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
)

endfunction()
```

```cmake
#config.cmake.in
# This file is used to create the final configuration file for find_package to locate the library
@PACKAGE_INIT@
set(LIB_NAME "@LIB_NAME@")

# Step 1: Set the name of the library to be found
# This makes it easier for find_package to locate the correct library target
set(${LIB_NAME}_LIBRARY_TO_FIND ${LIB_NAME})

# Step 2: Set and check the installation directories for the library
# These functions ensure that the installation paths for the library and include files are correct
set_and_check(${LIB_NAME}_LIB_DIR "@PACKAGE_LIB_INSTALL_DIR@")
set_and_check(${LIB_NAME}_INCLUDE_DIRS "@PACKAGE_INCLUDE_INSTALL_DIR@")

# Step 3: Include the target export file
# This file contains metadata for the installed library, allowing other projects to properly link against it
include("@PACKAGE_TARGET_EXPORT_FILE@")

# Step 4: Check for required components of the library
# Ensures that all necessary components are present when using find_package
check_required_components(${LIB_NAME})
```
