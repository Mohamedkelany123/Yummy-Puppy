include makefile.vars

greeting:
	@echo "This is ORM-C++ Project"

$(SOURCES): 
	@mkdir -p $(SOURCES)
	@echo $(SOURCES)

$(OBJECTS):
	@mkdir -p $(OBJECTS)
	@echo $(OBJECTS)

$(OBJECTS_MAIN):
	@mkdir -p $(OBJECTS_MAIN)
	@echo $(OBJECTS_MAIN)

$(OBJECTS_CMD_MAIN):
	@mkdir -p $(OBJECTS_CMD_MAIN)
	@echo $(OBJECTS_CMD_MAIN)

$(HEADERS):
	@mkdir -p $(HEADERS)
	@echo $(HEADERS)

$(BIN):
	@mkdir -p $(BIN)
	@echo $(BIN)

$(BIN_SERVICES):
	@mkdir -p $(BIN_SERVICES)
	@echo $(BIN_SERVICES)

$(DB_PRIMITIVE_ORM):
	@mkdir -p $(DB_PRIMITIVE_ORM)
	@echo $(DB_PRIMITIVE_ORM)

$(DB_PRIMITIVE_ORM_SOURCES):
	@mkdir -p $(DB_PRIMITIVE_ORM_SOURCES)
	@echo $(DB_PRIMITIVE_ORM_SOURCES)

$(DB_PRIMITIVE_ORM_HEADERS):
	@mkdir -p $(DB_PRIMITIVE_ORM_HEADERS)
	@echo $(DB_PRIMITIVE_ORM_HEADERS)

$(DB_BUSINESS_LOGIC_ORM):
	@mkdir -p $(DB_BUSINESS_LOGIC_ORM)
	@echo $(DB_BUSINESS_LOGIC_ORM)

$(DB_BUSINESS_LOGIC_ORM_SOURCES):
	@mkdir -p $(DB_BUSINESS_LOGIC_ORM_SOURCES)
	@echo $(DB_BUSINESS_LOGIC_ORM_SOURCES)

$(DB_BUSINESS_LOGIC_ORM_HEADERS):
	@mkdir -p $(DB_BUSINESS_LOGIC_ORM_HEADERS)
	@echo $(DB_BUSINESS_LOGIC_ORM_HEADERS)


build_tree: $(SOURCES) $(OBJECTS) $(OBJECTS_MAIN) $(OBJECTS_CMD_MAIN) $(HEADERS) $(BIN) $(BIN_SERVICES) $(DB_PRIMITIVE_ORM) $(DB_PRIMITIVE_ORM_SOURCES) $(DB_PRIMITIVE_ORM_HEADERS) $(DB_BUSINESS_LOGIC_ORM) $(DB_BUSINESS_LOGIC_ORM_SOURCES) $(DB_BUSINESS_LOGIC_ORM_HEADERS)
	@echo "Tree Built ....."

clean:
	rm -rf $(BIN)/* $(OBJECTS)/*.o $(OBJECTS_MAIN)/*

clean_primitive:
	rm -rf $(DB_PRIMITIVE_ORM_SOURCES)/* $(DB_PRIMITIVE_ORM_HEADERS)/*

SRC = $(wildcard $(SOURCES)/*.cpp)
OBJS = $(patsubst $(SOURCES)/%.cpp, $(OBJECTS)/%.cpp.o, $(SRC))

SRC_MAINS = $(wildcard $(SOURCES)/mains/*.cpp)
OBJS_MAINS = $(patsubst $(SOURCES)/mains/%.cpp, $(OBJECTS)/mains/%.cpp.o, $(SRC_MAINS))
MAIN_BINS = $(patsubst $(SOURCES)/mains/%.cpp, $(BIN)/%, $(SRC_MAINS))

SRC_CMD_MAINS = $(wildcard $(SOURCES)/mains/cmd_services/*.cpp)
OBJS_CMD_MAINS = $(patsubst $(SOURCES)/mains/cmd_services/%.cpp, $(OBJECTS)/mains/cmd_services/%.cpp.o, $(SRC_CMD_MAINS))
MAIN_CMD_BINS = $(patsubst $(SOURCES)/mains/cmd_services/%.cpp, $(BIN)/cmd_services/%, $(SRC_CMD_MAINS))

SRC_ABSTRACT = $(wildcard $(SOURCES)/abstract/*.cpp)
OBJS_ABSTRACT = $(patsubst $(SOURCES)/abstract/%.cpp, $(OBJECTS)/%.cpp.o, $(SRC_ABSTRACT))


SRC_POSTGRES = $(wildcard $(SOURCES)/postgres/*.cpp)
OBJS_POSTGRES = $(patsubst $(SOURCES)/postgres/%.cpp, $(OBJECTS)/%.cpp.o, $(SRC_POSTGRES))

SRC_FACTORY = $(wildcard $(DB_PRIMITIVE_ORM_SOURCES)/*.cpp)
OBJS_FACTORY = $(patsubst $(DB_PRIMITIVE_ORM_SOURCES)/%.cpp, $(OBJECTS)/%.cpp.o, $(SRC_FACTORY))

SRC_POSTGRES_COLUMN_TYPES = $(wildcard $(SOURCES)/postgres/column_types/*.cpp)
OBJS_POSTGRES_COLUMN_TYPES = $(patsubst $(SOURCES)/postgres/column_types/%.cpp, $(OBJECTS)/%.cpp.o, $(SRC_POSTGRES_COLUMN_TYPES))




$(OBJECTS)/%.cpp.o: $(SOURCES)/%.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(OBJECTS_MAIN)/%.cpp.o: $(SOURCES)/mains/%.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(OBJECTS_CMD_MAIN)/%.cpp.o: $(SOURCES)/mains/cmd_services/%.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(OBJECTS)/%.cpp.o: $(SOURCES)/abstract/%.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(OBJECTS)/%.cpp.o: $(SOURCES)/postgres/%.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(OBJECTS)/%.cpp.o: $(DB_PRIMITIVE_ORM_SOURCES)/%.cpp
	$(GCC) -D SKIP_ENTRY_POINT $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(OBJECTS)/%.cpp.o: $(SOURCES)/postgres/column_types/%.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) $< -o $@

# $(ORM_C_PLUS_PLUS): $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_POSTGRES_COLUMN_TYPES) $(ORM_C_PLUS_PLUS_OBJ)
# 	$(GCC) $(INCLUDES) $(LIBS) $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_POSTGRES_COLUMN_TYPES) $(ORM_C_PLUS_PLUS_OBJ) -o $(ORM_C_PLUS_PLUS) $(LINKER_FLAGS) 

.SECONDARY: $(OBJS_MAINS) $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_POSTGRES_COLUMN_TYPES) $(OBJS_FACTORY) $(OBJS_CMD_MAINS)

$(BIN)/cmd_services/%: $(OBJECTS)/mains/cmd_services/%.cpp.o $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_POSTGRES_COLUMN_TYPES) $(OBJS_FACTORY) 
	$(GCC) $(INCLUDES) $(LIBS) $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_POSTGRES_COLUMN_TYPES) $(OBJS_FACTORY) $(LINKER_FLAGS) $< -o $@

$(BIN)/%: $(SOURCES)/mains/%.cpp $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_POSTGRES_COLUMN_TYPES) 
	$(GCC) $(INCLUDES) $(LIBS) $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_POSTGRES_COLUMN_TYPES) $(LINKER_FLAGS) $< -o $@


# $(ORM_C++): $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_FACTORY) $(OBJS_POSTGRES_COLUMN_TYPES)
# 	$(GCC) $(INCLUDES) $(LIBS) $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_FACTORY) $(OBJS_POSTGRES_COLUMN_TYPES) -o $(ORM_C++) $(LINKER_FLAGS) 

#$(MAIN_BINS)
orm_c++: build_tree  $(MAIN_BINS)
	@echo "Generating Executable"

services: build_tree  $(MAIN_CMD_BINS)
	@echo "Generating Executable"
