#ifndef PSQLPRIMITIVEORMGENERATOR_H
#define PSQLPRIMITIVEORMGENERATOR_H

#include <PSQLController.h>
#include <PSQLInt2.h>
#include <PSQLInt4.h>
#include <PSQLInt8.h>
#include <PSQLBool.h>
#include <PSQLText.h>
#include <PSQLJson.h>
#include <PSQLNumeric.h>

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
        string primary_key;
        char * template_h;
        char * template_cpp;
        char * h_file;
        char * cpp_file;
        PSQLConnection * psqlConnection;
        map <string,AbstractDatabaseColumn *> databaseColumnFactory;
        void get_primary_key(string table_name);
        void generateDecl_Setters_Getters (string class_name,map<string, vector<string>> columns_definition);
        void generateFromString (string class_name,string table_name,string table_index,map<string, vector<string>> columns_definition);
        void generateAssignResults (string class_name,string table_name,map<string, vector<string>> columns_definition);
        void generateFieldsMap (string class_name,string table_name,map<string, vector<string>> columns_definition);
        void generateAssignmentOperator (string class_name,string table_name,map<string, vector<string>> columns_definition);
        void generateGetIdentifier(string class_name);
        void generateExternDSOEntryPoint (string class_name,string table_name);
        void generateConstructorAndDestructor(string class_name,string table_name);
        void generateAddToCache(string class_name);
        void generateIsUpdated(string class_name);
        void generateCloner(string class_name);
        void generateUpdateQuery(string class_name,string table_name,map<string, vector<string>> columns_definition);
        void generateInsertQuery(string class_name,string table_name,map<string, vector<string>> columns_definition);
        void generateSerializer(string class_name,string table_name,map<string, vector<string>> columns_definition);
        void generateDeserializer(string class_name,string table_name,map<string, vector<string>> columns_definition);
        void generateEqualToOperator(string class_name,string table_name,map<string, vector<string>> columns_definition);
        void fetch_templates();
        void write_headers_and_sources(string class_name);
    public:
        PSQLPrimitiveORMGenerator();
        void generate(string table_name,string table_index);
        void compile(string table_name);
        ~PSQLPrimitiveORMGenerator();
};

#endif