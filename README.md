# ORM-C_plus_plus

To clone current repo with all submodules:
```
git clone --recurse-submodules <CURRENT_REPO_URL>
```

## Docs
- Check [coding convention doc](coding_conventions.md) for coding conventions and styles being used
- Check [CMake wiki doc](cmake_wiki.md) for CMake guidelines and common CMake use-cases 

## Compiling framework using CMake

Folders to be compiled & installed: *external_deps, framework, webserver, and middlewares*

1. Generate CMake configuration files
    ```
    cmake . -B build
    ```
    
    - For debugging, 
    ```
    cmake -DCMAKE_BUILD_TYPE=Debug . -B build
    ```

2. Compiling using generated makefile
    ```
    cmake --build build -j10
    ```

3. Installing compiled files to system files
    ```
    sudo cmake --install build
    ```

* Build files will be inside `build` directory
* Installed files will be installed to system libs, includes, & bin

## Generating & Compiling factory

- Edit `orm_config.json` 
    - database info
    - resticted tables to generate
    - namespace to generate factory with
    - directory to generate factory in
    - directory of templates (either local or from system files)

- Framework must be installed to system files

1. Use generator executable installed in system bin 
    ```
    orm_generator generate <path/to>/orm_config.json
    ```

2. Go to generated orm folder

3. Compile & install generated orms 
    ```
    cmake . -B build
    cmake --build build -j10
    sudo cmake --install build
    ```
    - For debugging, 
    ```
    cmake -DCMAKE_BUILD_TYPE=Debug . -B build
    cmake --build build -j10
    sudo cmake --install build
    ```

## For compiling applications

> Each application has different dependencies that must be installed before compiling. CMake specifies if compilation will fail because dependencies are not met. 

*Each application follows different CMake structure but they should be compiled using same commands*

1. Go to application folder
2. Compile application 
    ```
    cmake . -B build
    cmake --build build -j10
    ```
    - For debugging, 
    ```
    cmake -DCMAKE_BUILD_TYPE=Debug . -B build
    cmake --build build -j10
    ```


## Applications

> DO NOT USE `git submodule init` and `git submodule update` without specifying which application unless you have access to all applications

### Available applications:
- [blnk_closure](https://github.com/blnk-ai/blnk_closure)
- [LOS](https://github.com/blnk-ai/LOS)
- [tally_dashboard_services](https://github.com/blnk-ai/tally_dashboard_services)
- [orm_c_plus_plus_misc](https://github.com/blnk-ai/orm_c_plus_plus_misc)


### To clone submodule:
```
git submodule init applications/<application_name>
git submodule update applications/<application_name>
```
### To remove submodule:
```
git submodule deinit applications/<application_name>
```

### To add new application (submodule):
```
git submodule add <REPO_URL> applications/<application_name>
```

---