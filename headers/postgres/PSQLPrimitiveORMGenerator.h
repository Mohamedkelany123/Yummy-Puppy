#ifndef PSQLPRIMITIVEORMGENERATOR_H
#define PSQLPRIMITIVEORMGENERATOR_H

#include <PSQLConnection.h>
#include <PSQLInt4.h>
#include <PSQLInt8.h>

#define TEMPLATE_H_FILENAME "./templates/headers/ORMPrimitiveTemplate.h"
#define TEMPLATE_CPP_FILENAME "./templates/sources/ORMPrimitiveTemplate.cpp"
#define SOURCES_H_FILENAME "./factory/db_primitive_orm/headers/"
#define SOURCES_CPP_FILENAME "./factory/db_primitive_orm/sources/"

#define MAX_SOURCE_FILE_SIZE 5*1024*1024
class PSQLPrimitiveORMGenerator
{
    private:
        string declaration;
        string setters;
        string getters;
        string setters_def;
        string getters_def;
        string extra_methods_def;
        string extra_methods;
        string from_string;
        string assign_results;
        string extern_entry_point;
        string constructor_destructor_def;
        string constructor_destructor;
        string includes;
        char * template_h;
        char * template_cpp;
        char * h_file;
        char * cpp_file;
        PSQLConnection * psqlConnection;
        map <string,AbstractDatabaseColumn *> databaseColumnFactory;
        void generateDecl_Setters_Getters (string class_name,map<string, vector<string>> columns_definition);
        void generateFromString (string class_name,map<string, vector<string>> columns_definition);
        void generateAssignResults (string class_name,map<string, vector<string>> columns_definition);
        void generateExternDSOEntryPoint (string class_name,string table_name);
        void generateConstructorAndDestructor(string class_name,string table_name);
        void fetch_templates();
        void write_headers_and_sources(string class_name);
    public:
        PSQLPrimitiveORMGenerator();
        void generate(string table_name);
        void compile(string table_name);
        ~PSQLPrimitiveORMGenerator();
};

#endif