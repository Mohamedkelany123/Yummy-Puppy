include makefile.vars

greeting:
	@echo "This is ORM-C++ Project"

$(SOURCES): 
	@mkdir -p $(SOURCES)
	@echo $(SOURCES)

$(OBJECTS):
	@mkdir -p $(OBJECTS)
	@echo $(OBJECTS)

$(HEADERS):
	@mkdir -p $(HEADERS)
	@echo $(HEADERS)

$(BIN):
	@mkdir -p $(BIN)
	@echo $(BIN)

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

build_tree: $(SOURCES) $(OBJECTS) $(HEADERS) $(BIN) $(DB_PRIMITIVE_ORM) $(DB_PRIMITIVE_ORM_SOURCES) $(DB_PRIMITIVE_ORM_HEADERS) $(DB_BUSINESS_LOGIC_ORM) $(DB_BUSINESS_LOGIC_ORM_SOURCES) $(DB_BUSINESS_LOGIC_ORM_HEADERS)
	@echo "Tree Built ....."

clean:
	rm -rf $(BIN)/* $(OBJECTS)/*

SRC = $(wildcard $(SOURCES)/*.cpp)
OBJS = $(patsubst $(SOURCES)/%.cpp, $(OBJECTS)/%.cpp.o, $(SRC))


SRC_MAINS = $(wildcard $(SOURCES)/mains/*.cpp)
OBJS_MAINS = $(patsubst $(SOURCES)/mains/%.cpp, $(OBJECTS)/%.cpp.o, $(SRC_MAINS))

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

$(OBJECTS)/%.cpp.o: $(SOURCES)/mains/%.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(OBJECTS)/%.cpp.o: $(SOURCES)/abstract/%.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(OBJECTS)/%.cpp.o: $(SOURCES)/postgres/%.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(OBJECTS)/%.cpp.o: $(SOURCES)/postgres/%.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(OBJECTS)/%.cpp.o: $(DB_PRIMITIVE_ORM_SOURCES)/%.cpp
	$(GCC) -D SKIP_ENTRY_POINT $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(OBJECTS)/%.cpp.o: $(SOURCES)/postgres/column_types/%.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(ORM_C_PLUS_PLUS): $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_POSTGRES_COLUMN_TYPES) $(ORM_C_PLUS_PLUS_OBJ)
	$(GCC) $(INCLUDES) $(LIBS) $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_POSTGRES_COLUMN_TYPES) $(ORM_C_PLUS_PLUS_OBJ) -o $(ORM_C_PLUS_PLUS) $(LINKER_FLAGS) 

# $(ORM_C++): $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_FACTORY) $(OBJS_POSTGRES_COLUMN_TYPES)
# 	$(GCC) $(INCLUDES) $(LIBS) $(OBJS) $(OBJS_ABSTRACT) $(OBJS_POSTGRES) $(OBJS_FACTORY) $(OBJS_POSTGRES_COLUMN_TYPES) -o $(ORM_C++) $(LINKER_FLAGS) 


orm_c++: build_tree $(ORM_C_PLUS_PLUS)
	@echo "Generating Executable"
