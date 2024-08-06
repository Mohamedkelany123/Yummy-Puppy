# ORM-C_plus_plus

To clone current repo with all submodules:
```
git clone --recurse-submodules <CURRENT_REPO_URL>
```

## Generate ORMS

1. compile generator executable

    `make orm_c++`
2. edit database info, tables to generate, and factory directory in orm_config.json file
3. generate orms

    `<path/to>/orm_c++ generate <path/to>/orm_config.json`
4. clean orms

    `<path/to>/orm_c++ clean <path/to>/orm_config.json`


## Available Applications

> DO NOT USE `git submodule init` and `git submodule update` without specifying which application unless you have access to all applications

### 1. blnk_closure

- [Repo Link](https://github.com/blnk-ai/blnk_closure)
- To clone submodule:
```
git submodule init applications/blnk_closure
git submodule update applications/blnk_closure
```

### 2. tally_dashboard_services

- [Repo Link](https://github.com/blnk-ai/tally_dashboard_services)
- To clone submodule:
```
git submodule init applications/tally_dashboard_services
git submodule update applications/tally_dashboard_services
```

### 3. orm_c_plus_plus_misc

- [Repo Link](https://github.com/blnk-ai/orm_c_plus_plus_misc)
- To clone submodule:
```
git submodule init applications/orm_c_plus_plus_misc
git submodule update applications/orm_c_plus_plus_misc
```
### To add new application (submodule):

```
git submodule add <REPO_URL> applications/
```

---