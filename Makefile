all:
	$(MAKE) -f recursive_makefile all

clean:
	$(MAKE) -f recursive_makefile clean

closure_go: 
	bash closure_go.sh

orm_c++:
	$(MAKE) -C sources/abstract make_target_cpp
	$(MAKE) -C sources/column_types make_target_cpp
	$(MAKE) -C sources/postgres make_target_cpp
	$(MAKE) -C sources/utils make_target_cpp
	$(MAKE) -C sources/mains/orm_generator make_target_cpp
	$(MAKE) -C sources/mains/orm_generator -f bin_makefile  all

build_factory:
	@echo "Setting up factory"
	$(MAKE) -C factory

clean_primitive:
	rm -rf ./factory/db_primitive_orm/headers/*.h
	rm -rf ./factory/db_primitive_orm/sources/*.cpp