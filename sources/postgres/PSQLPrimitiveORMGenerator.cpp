#include <PSQLPrimitiveORMGenerator.h>



void PSQLPrimitiveORMGenerator::generateSerializer(string class_name,string table_name,map<string, vector<string>> columns_definition)
{
    extra_methods_def += "\t\tstring serialize (PSQLConnection * _psqlConnection=NULL);\n";
    extra_methods += "\t\tstring "+class_name+"::serialize (PSQLConnection * _psqlConnection){\n";
    extra_methods += "\t\t\tstring serialize_string = \"\\\""+class_name+"\\\":{\";\n";
    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
            string orm_field_name = "orm_"+columns_definition["column_name"][i];

            bool string_flag = false;
            for ( int j = 0 ; PSQLText::get_native_type(j) != "" ; j ++)
                if ( PSQLText::get_native_type(j) == columns_definition["udt_name"][i]) string_flag=true;


            bool bool_flag = false;
            for ( int j = 0 ; PSQLBool::get_native_type(j) != "" ; j ++)
                if ( PSQLBool::get_native_type(j) == columns_definition["udt_name"][i]) bool_flag=true;

            bool json_flag = false;
            for ( int j = 0 ; PSQLJson::get_native_type(j) != "" ; j ++)
                if ( PSQLJson::get_native_type(j) == columns_definition["udt_name"][i]) json_flag=true;

            if (string_flag )
                extra_methods += "\t\t\tserialize_string += \"\\\""+orm_field_name+"\\\":\\\"\" +"+orm_field_name+"+\"\\\"\";\n";
            else if (json_flag )
                extra_methods += "\t\t\tserialize_string += \"\\\""+orm_field_name+"\\\":\" +"+orm_field_name+".dump();\n";
            else if (bool_flag )
            {
                extra_methods += "\t\t\tif ("+orm_field_name+") serialize_string += \"\\\""+orm_field_name+"\\\": true\"; else serialize_string += \"\\\""+orm_field_name+"\\\": false\";\n";
            }
            else extra_methods += "\t\t\tserialize_string += \"\\\""+orm_field_name+"\\\":\" +std::to_string("+orm_field_name+");\n";

            if ( i < columns_definition["column_name"].size() -1)
                extra_methods += "\t\t\tserialize_string += \",\";\n";
            extra_methods += "\t\t\tserialize_string += \"\\n\";\n";
    }

    extra_methods += "\t\t\tserialize_string += \"}\";\n";    
    extra_methods += "\t\t\t\treturn serialize_string;\n";
    extra_methods += "\t\t}\n";

}

void PSQLPrimitiveORMGenerator::generateDeserializer(string class_name,string table_name,map<string, vector<string>> columns_definition)
{

    extra_methods_def += "\t\tvoid deSerialize (json orm_json,bool _read_only = false);\n";
    extra_methods += "\t\tvoid "+class_name+"::deSerialize (json orm_json,bool _read_only){\n";
    // extra_methods += "\t\t\tpsqlQuery->fetchNextRow();\n";
    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        string orm_field_name = "orm_"+columns_definition["column_name"][i];
        bool string_flag = false;
        for ( int j = 0 ; PSQLText::get_native_type(j) != "" ; j ++)
            if ( PSQLText::get_native_type(j) == columns_definition["udt_name"][i]) string_flag=true;

        bool json_flag = false;
        for ( int j = 0 ; PSQLJson::get_native_type(j) != "" ; j ++)
            if ( PSQLJson::get_native_type(j) == columns_definition["udt_name"][i]) json_flag=true;

        if (databaseColumnFactory.find(columns_definition["udt_name"][i]) != databaseColumnFactory.end()) {
            AbstractDatabaseColumn * abstractDatabaseColumn = databaseColumnFactory[columns_definition["udt_name"][i]]->clone(columns_definition["column_name"][i]);
            extra_methods += "\t\t\torm_"+columns_definition["column_name"][i];
                extra_methods += " = orm_json[\""+orm_field_name+"\"];\n";
            // else extra_methods += " = " +abstractDatabaseColumn->genFieldConversion("std::to_string(orm_json[\""+orm_field_name+"\"])")+ ";\n";
//            else extra_methods += " = " +abstractDatabaseColumn->genFieldConversion("psqlQuery->getValue(\"" +table_name+"_"+ columns_definition["column_name"][i]+"\")")+ ";\n";
            delete(abstractDatabaseColumn);
        }
    }
    extra_methods += "\t\t\tloaded=true;\n";
    extra_methods += "\t\t\tif (!_read_only) addToCache();\n";
    extra_methods += "\t\t}\n";
}

void PSQLPrimitiveORMGenerator::generateEqualToOperator(string class_name,string table_name,map<string, vector<string>> columns_definition)
{
    extra_methods_def += "\t\tbool operator == ("+class_name+" & "+class_name+"_object );\n";
    extra_methods += "\t\tbool "+class_name+"::operator == ("+class_name+" & "+class_name+"_object ){\n";
    // extra_methods += "\t\t\tpsqlQuery->fetchNextRow();\n";
    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        bool string_flag = false;
        for ( int j = 0 ; PSQLText::get_native_type(j) != "" ; j ++)
            if ( PSQLText::get_native_type(j) == columns_definition["udt_name"][i]) string_flag=true;

        bool json_flag = false;
        for ( int j = 0 ; PSQLJson::get_native_type(j) != "" ; j ++)
            if ( PSQLJson::get_native_type(j) == columns_definition["udt_name"][i]) json_flag=true;

        string orm_field_name = "orm_"+columns_definition["column_name"][i];

        if (!string_flag && !json_flag)
            extra_methods += "\t\t\tif (!(std::to_string(this->"+orm_field_name+") == std::to_string("+class_name+"_object."+orm_field_name+") )) {cout << \""+orm_field_name+"\" << endl; return false;}\n";
        else extra_methods += "\t\t\tif (!(this->"+orm_field_name+" == "+class_name+"_object."+orm_field_name+" )) {cout << \""+orm_field_name+"\" << endl; return false;}\n";
    }
    extra_methods += "\t\t\treturn true; \n";
    extra_methods += "\t\t}\n";

}


void PSQLPrimitiveORMGenerator::fetch_templates()
{
    FILE * f = fopen (TEMPLATE_H_FILENAME,"rt");
    if ( f != NULL )
    {
        fseek (f,0,2);
        long filesize = ftell(f);
        fseek (f,0,0);
        template_h  = (char *) calloc (filesize+1,sizeof(char));
        fread(template_h,1,filesize,f);
        fclose(f);
    }
    f = fopen (TEMPLATE_CPP_FILENAME,"rt");
    if ( f != NULL )
    {
        fseek (f,0,2);
        long filesize = ftell(f);
        fseek (f,0,0);
        template_cpp  = (char *) calloc (filesize+1,sizeof(char));
        fread(template_cpp,1,filesize,f);
        fclose(f);
    }

}

void PSQLPrimitiveORMGenerator::write_headers_and_sources(string class_name)
{
    string lower_case= "";
    for(auto& c : class_name) lower_case += tolower(c);
    class_name= lower_case;

    string h_file_name = this->orm_folder + H_FOLDER + "/" + class_name + ".h";
    string cpp_file_name = this->orm_folder + CPP_FOLDER + "/" + class_name + ".cpp";
    
    FILE * f = fopen (h_file_name.c_str(),"wt");
    if ( f != NULL )
    {
        fwrite (h_file,1,strlen(h_file),f);
        fclose(f);
    }

    f = fopen (cpp_file_name.c_str(),"wt");
    if ( f != NULL )
    {
        fwrite (cpp_file,1,strlen(cpp_file),f);
        fclose(f);
    }
}


PSQLPrimitiveORMGenerator::PSQLPrimitiveORMGenerator(string p_datasource, string p_orm_folder)
{
    this->orm_folder = p_orm_folder;
    this->datasource = p_datasource;

    PSQLPrimitiveORMGenerator::createFoldersIfNotExist(orm_folder+H_FOLDER);
    PSQLPrimitiveORMGenerator::createFoldersIfNotExist(orm_folder+CPP_FOLDER);

    psqlConnection = psqlController.getPSQLConnection(datasource);
    for ( int i = 0 ; PSQLInt2::get_native_type(i) != "" ; i ++)
        databaseColumnFactory[PSQLInt2::get_native_type(i)] = new PSQLInt2();
    for ( int i = 0 ; PSQLInt4::get_native_type(i) != "" ; i ++)
        databaseColumnFactory[PSQLInt4::get_native_type(i)] = new PSQLInt4();
    for ( int i = 0 ; PSQLInt8::get_native_type(i) != "" ; i ++)
        databaseColumnFactory[PSQLInt8::get_native_type(i)] = new PSQLInt8();
    for ( int i = 0 ; PSQLBool::get_native_type(i) != "" ; i ++)
        databaseColumnFactory[PSQLBool::get_native_type(i)] = new PSQLBool();
    for ( int i = 0 ; PSQLText::get_native_type(i) != "" ; i ++)
        databaseColumnFactory[PSQLText::get_native_type(i)] = new PSQLText();
    for ( int i = 0 ; PSQLJson::get_native_type(i) != "" ; i ++)
        databaseColumnFactory[PSQLJson::get_native_type(i)] = new PSQLJson();
    for ( int i = 0 ; PSQLNumeric::get_native_type(i) != "" ; i ++)
        databaseColumnFactory[PSQLNumeric::get_native_type(i)] = new PSQLNumeric();
    template_h = NULL;
    template_cpp = NULL;
    h_file = (char *) calloc (MAX_SOURCE_FILE_SIZE,sizeof(char));
    cpp_file = (char *) calloc (MAX_SOURCE_FILE_SIZE,sizeof(char));
    fetch_templates();
}
void PSQLPrimitiveORMGenerator::get_primary_key(string table_name,bool is_view){
    primary_key = "";
    if (is_view)
    {   
        if (view_serial_flag ) primary_key = "view_serial";
        return;
    }
    AbstractDBQuery *psqlQuery = psqlConnection->executeQuery(R""""(SELECT 
        pg_attribute.attname, 
        format_type(pg_attribute.atttypid, pg_attribute.atttypmod) 
        FROM pg_index, pg_class, pg_attribute, pg_namespace 
        WHERE 
        pg_class.oid = '")""""+table_name+R""""("'::regclass AND 
        indrelid = pg_class.oid AND 
        nspname = 'public' AND 
        pg_class.relnamespace = pg_namespace.oid AND 
        pg_attribute.attrelid = pg_class.oid AND 
        pg_attribute.attnum = any(pg_index.indkey)
        AND indisprimary)"""");
    if (psqlQuery->fetchNextRow())
    {
        if ( psqlQuery->getValue("format_type") == "integer" || psqlQuery->getValue("format_type") == "bigint")
            primary_key = psqlQuery->getValue("attname");
        else cout << table_name << " ->>>>>>>>> " << psqlQuery->getValue("format_type") << endl;
    }
    delete (psqlQuery);

}
void PSQLPrimitiveORMGenerator::generateDecl_Setters_Getters (string class_name,map<string, vector<string>> columns_definition)
{
    declaration = "";
    setters = "";
    getters = "";
    setters_def = "";
    getters_def = "";
    int col_count = columns_definition["column_name"].size();
    
    declaration += "\t\tstatic mutex "+class_name+"_lock;\n";
    declaration += "\t\tstatic string "+class_name+"_locking_thread;\n";

    declaration += "\t\tbitset<";
    declaration +=std::to_string(col_count);
    declaration += "> update_flag;\n";
    declaration += "\t\tbitset<";
    declaration +=std::to_string(col_count);
    declaration += "> null_flag;\n";

    declaration += "\t\tbitset<";
    declaration +=std::to_string(col_count);
    declaration += "> field_clear_mask_flag;\n";

    //Static lock and unlock for all orm objects
    setters_def += "\t\tvoid static_lock(bool skip_owner = false); \n";
    setters_def += "\t\tvoid static_unlock(bool restrict_to_owner = false); \n";
    //static var decleration in cpp
    setters += "\t\tmutex "+class_name+"::"+class_name+"_lock;\n";
    setters += "\t\tstring "+class_name+"::"+class_name+"_locking_thread;\n";

    //Static_lock
    setters +=  "\t\tvoid "+class_name+"::static_lock(bool skip_owner){\n";
    setters += "\t\t\tstd::ostringstream ss;\n";
    setters += "\t\t\tss << std::this_thread::get_id();\n";
    setters += "\t\t\tif (skip_owner && ss.str() == "+class_name+"_locking_thread) return;\n";    
    setters += "\t\t\t"+class_name+"_lock.lock();\n";
    setters += "\t\t\t"+class_name+"_locking_thread = ss.str();\n";
    setters += "\t\t}\n";

    //Static_unlock
    setters +=  "\t\tvoid "+class_name+"::static_unlock(bool restrict_to_owner){\n";
    setters += "\t\t\tif ( "+class_name+"_locking_thread == \"\") return ;\n";
    setters += "\t\t\tstd::ostringstream ss;\n";
    setters += "\t\t\tss << std::this_thread::get_id();\n";
    setters += "\t\t\tif (restrict_to_owner)\n";
    setters += "\t\t\t\tif (ss.str() != "+class_name+"_locking_thread) return;\n";
    setters += "\t\t\t"+class_name+"_locking_thread = \"\";\n";
    setters += "\t\t\t"+class_name+"_lock.try_lock();\n";
    setters += "\t\t\t"+class_name+"_lock.unlock();\n";
    setters += "\t\t}\n";

    for (int i  = 0 ; i  < col_count; i++) 
    {
        if (databaseColumnFactory.find(columns_definition["udt_name"][i]) != databaseColumnFactory.end()) {
            AbstractDatabaseColumn * abstractDatabaseColumn = databaseColumnFactory[columns_definition["udt_name"][i]]->clone(columns_definition["column_name"][i]);
            declaration += abstractDatabaseColumn->genDeclaration();
            setters += abstractDatabaseColumn->genSetter(class_name,i);
            getters += abstractDatabaseColumn->genGetter(class_name);
            setters_def += abstractDatabaseColumn->genSetterDef();
            getters_def += abstractDatabaseColumn->genGetterDef();
            delete(abstractDatabaseColumn);
        }
        else cout << "TYPE: " << columns_definition["udt_name"][i] << " - " << columns_definition["numeric_precision"][i] << " - " << columns_definition["numeric_precision_radix"][i] << " - "<< columns_definition["numeric_scale"][i] << endl;
    }
}

void PSQLPrimitiveORMGenerator::generateFromString (string class_name,string table_name,string table_index,map<string, vector<string>> columns_definition)
{
    extra_methods_def += "\t\tstring getFromString ();\n";
    extra_methods += "\t\tstring "+class_name+"::getFromString (){\n";
    extra_methods += "\t\t\tif (!field_clear_mask_flag.any() ) return \"";
    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        if ( i > 0 )  extra_methods  += ",";
        extra_methods += "\\\""+table_name+"\\\".\\\""+columns_definition["column_name"][i]+"\\\" as \\\""+table_index+"_"+columns_definition["column_name"][i]+"\\\"";
    }
    extra_methods += "\";\n";
    extra_methods += "\t\t\telse {\n";
    extra_methods += "\t\t\t\tstring f_str=\"\";\n";
    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        extra_methods  += "\t\t\t\tif (field_clear_mask_flag.test("+to_string(i)+")) {\n";
        extra_methods  += "\t\t\t\tif (f_str != \"\" ) f_str +=\",\";\n ";
        extra_methods += "\t\t\t\tf_str +=\"\\\""+table_name+"\\\".\\\""+columns_definition["column_name"][i]+"\\\" as \\\""+table_index+"_"+columns_definition["column_name"][i]+"\\\"\";\n";
        extra_methods += "\t\t\t\t}\n";
    }

    extra_methods += "\t\t\t\treturn f_str;\n";    
    extra_methods += "\t\t\t}\n";    
    extra_methods += "\n\t\t}\n";
}

void PSQLPrimitiveORMGenerator::generateAssignResults (string class_name,string table_name,map<string, vector<string>> columns_definition,bool is_view)
{
    extra_methods_def += "\t\tvoid assignResults (AbstractDBQuery * psqlQuery,bool _read_only = false);\n";
    extra_methods += "\t\tvoid "+class_name+"::assignResults (AbstractDBQuery * psqlQuery,bool _read_only){\n";
    // extra_methods += "\t\t\tpsqlQuery->fetchNextRow();\n";
    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        if (databaseColumnFactory.find(columns_definition["udt_name"][i]) != databaseColumnFactory.end()) {
            AbstractDatabaseColumn * abstractDatabaseColumn = databaseColumnFactory[columns_definition["udt_name"][i]]->clone(columns_definition["column_name"][i]);
            extra_methods += "\t\t\torm_"+columns_definition["column_name"][i];
            if ( columns_definition["udt_name"][i] == "jsonb")
                extra_methods += " = " +abstractDatabaseColumn->genFieldConversion("psqlQuery->getJSONValue(\"" +table_name+"_"+ columns_definition["column_name"][i]+"\")",i)+ ";\n";            
            else extra_methods += " = " +abstractDatabaseColumn->genFieldConversion("psqlQuery->getValue(\"" +table_name+"_"+ columns_definition["column_name"][i]+"\")",i)+ ";\n";
            delete(abstractDatabaseColumn);
        }
    }
    extra_methods += "\t\t\tloaded=true;\n";
    if ( !is_view) {
        extra_methods += "\t\t\tinserted=true;\n";
        extra_methods += "\t\t\tif (!_read_only) {\n";
        extra_methods += "\t\t\t\taddToCache();\n";
        extra_methods += "\t\t\t\tcached=true;\n";
        extra_methods += "\t\t\t}\n";
    }   
    extra_methods += "\t\t}\n";
}



void PSQLPrimitiveORMGenerator::generateFieldsMap (string class_name,string table_name,map<string, vector<string>> columns_definition)
{
    extra_methods_def += "\t\tmap<string,string> generateFieldsMap ();\n";
    extra_methods += "\t\tmap<string,string> "+class_name+"::generateFieldsMap (){\n";
    extra_methods += "\t\t\tmap <string,string> fields_map;\n";
    view_serial_flag=false;
    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        string db_field_name = columns_definition["column_name"][i];
        if (db_field_name == "view_serial") view_serial_flag=true;
        string orm_field_name = "orm_"+columns_definition["column_name"][i];
        bool string_flag = false;
        for ( int j = 0 ; PSQLText::get_native_type(j) != "" ; j ++)
            if ( PSQLText::get_native_type(j) == columns_definition["udt_name"][i]) string_flag=true;

        bool json_flag = false;
        for ( int j = 0 ; PSQLJson::get_native_type(j) != "" ; j ++)
            if ( PSQLJson::get_native_type(j) == columns_definition["udt_name"][i]) json_flag=true;

            
        if(string_flag)
            extra_methods += "\t\t\t\t\tfields_map[\""+orm_field_name+"\"]= "+orm_field_name+";\n";
        else if (json_flag)
            extra_methods += "\t\t\t\t\tfields_map[\""+orm_field_name+"\"]= "+orm_field_name+".dump();\n";
        else extra_methods += "\t\t\t\t\tfields_map[\""+orm_field_name+"\"]= std::to_string("+orm_field_name+");\n";        
    }

    extra_methods += "\t\t\treturn fields_map;\n";
    extra_methods += "\t\t}\n";
}


void PSQLPrimitiveORMGenerator::generateAssignmentOperator (string class_name,string table_name,map<string, vector<string>> columns_definition)
{
    extra_methods_def += "\t\tvoid operator = (const "+class_name+" & orm);\n";
    extra_methods += "\t\tvoid "+class_name+"::operator = (const "+class_name+" & orm){\n";


    extra_methods += "\t\t\t *((PSQLAbstractORM *)this) = orm;\n";
    extra_methods += "\t\t\tthis->orm_"+primary_key+"= orm.orm_"+primary_key+";\n";
    extra_methods += "\t\t\tthis->table_index=orm.table_index;\n";
    extra_methods += "\t\t\tcached=orm.cached;\n";

    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        if (databaseColumnFactory.find(columns_definition["udt_name"][i]) != databaseColumnFactory.end()) {
            extra_methods += "\t\t\torm_"+columns_definition["column_name"][i];
            extra_methods += " = orm.orm_" +columns_definition["column_name"][i]+ ";\n";            
        }
    }
    extra_methods += "\t\t\tloaded=orm.loaded;\n";
    extra_methods += "\t\t\tupdate_flag=orm.update_flag;\n";
    extra_methods += "\t\t\tnull_flag=orm.null_flag;\n";
    extra_methods += "\t\t\tfield_clear_mask_flag=orm.field_clear_mask_flag;\n";
    extra_methods += "\t\t\tfor (size_t   i = 0 ; i< update_flag.size() ; i ++) update_flag.set(i) ;\n";
    extra_methods += "\t\t\tnull_flag=orm.null_flag;\n";
    extra_methods += "\t\t\tsetIdentifier(-1);\n";
    extra_methods += "\t\t\tif (cached) this->addToCache();\n";

    extra_methods += "\t\t}\n";

    extra_methods_def += "\t\tvoid operator = (const "+class_name+" * orm); // This is a special opertaor for the cache only\n";
    extra_methods += "\t\tvoid "+class_name+"::operator = (const "+class_name+" * orm){\n";
    extra_methods += "\t\t\t *((PSQLAbstractORM *)this) = orm;\n";
    extra_methods += "\t\t\tthis->orm_"+primary_key+"= orm->orm_"+primary_key+";\n";
    extra_methods += "\t\t\tthis->table_index=orm->table_index;\n";
    extra_methods += "\t\t\tcached=orm->cached;\n";

    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        if (databaseColumnFactory.find(columns_definition["udt_name"][i]) != databaseColumnFactory.end()) {
            extra_methods += "\t\t\torm_"+columns_definition["column_name"][i];
            extra_methods += " = orm->orm_" +columns_definition["column_name"][i]+ ";\n";            
        }
    }
    extra_methods += "\t\t\tloaded=orm->loaded;\n";
    extra_methods += "\t\t\tupdate_flag=orm->update_flag;\n";
    extra_methods += "\t\t\tfield_clear_mask_flag=orm->field_clear_mask_flag;\n";
    extra_methods += "\t\t\tnull_flag=orm->null_flag;\n";
    extra_methods += "\t\t}\n";




}

void PSQLPrimitiveORMGenerator::generateGetIdentifier(string class_name)
{
    extra_methods_def += "\t\tlong getIdentifier ();\n";
    extra_methods += "\t\tlong "+class_name+"::getIdentifier (){\n";
    extra_methods += "\t\t\treturn orm_"+primary_key+";\n";
    extra_methods += "\t\t}\n";
    extra_methods_def += "\t\tvoid setIdentifier (long id);\n";
    extra_methods += "\t\tvoid "+class_name+"::setIdentifier (long id){\n";
    extra_methods += "\t\t\t orm_"+primary_key+" = id;\n";
    extra_methods += "\t\t}\n";
}
void PSQLPrimitiveORMGenerator::generateCloner(string class_name)
{
    extra_methods_def += "\t\tPSQLAbstractORM * clone ();\n";
    extra_methods += "\t\tPSQLAbstractORM * "+class_name+"::clone (){\n";
    extra_methods += "\t\t\treturn new "+class_name+"(data_source_name,false,true,-1, {},seeder_readonly);\n";
    extra_methods += "\t\t}\n";


}



void PSQLPrimitiveORMGenerator::generateExternDSOEntryPoint (string class_name,string table_name)
{
    extern_entry_point ="#ifndef SKIP_ENTRY_POINT\n";
    extern_entry_point +="extern \"C\" PSQLAbstractORM *create_object() {\n";
    extern_entry_point +="\treturn new "+class_name+"();\n";
    extern_entry_point += "}\n";
    extern_entry_point += "#endif";
}

void PSQLPrimitiveORMGenerator::generateConstructorAndDestructor(string class_name,string table_name,string table_index, map<string, vector<string>> columns_definition, vector<string> & tables_to_generate)
{
    includes = "#include <PSQLController.h>\n";
    includes += "#include <PSQLBool.h>\n";
    constructor_destructor = "\t\t"+class_name+"::"+class_name+"(string _data_source_name,bool add_to_cache, bool orm_transactional,int _enforced_partition_number, vector <string> _field_clear_mask,bool _seeder_readonly):PSQLAbstractORM(_data_source_name,\""+table_name+"\",\""+primary_key+"\", orm_transactional,_enforced_partition_number,_field_clear_mask,_seeder_readonly){\n";
    constructor_destructor += "\t\t\torm_"+primary_key+"=-1;\n";
    constructor_destructor += "\t\t\ttable_index="+table_index+";\n";
    constructor_destructor += "\t\t\tcached=add_to_cache;\n";
    constructor_destructor += "\t\t\tenforced_partition_number=_enforced_partition_number;\n";

    constructor_destructor += "\t\t\tif (add_to_cache) this->addToCache();\n";

    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        constructor_destructor  += "\t\t\tif (std::find(field_clear_mask.begin(), field_clear_mask.end(), \""+columns_definition["column_name"][i]+"\") != field_clear_mask.end()) field_clear_mask_flag.set("+to_string(i)+");\n";
    }


    // constructor_destructor +="\t\t\tpsqlQuery = psqlConnection->executeQuery(\"select \"+this->getFromString()+\" from "+table_name+"\");\n";
    AbstractDBQuery *psqlQuery = psqlConnection->executeQuery(R""""(select * from ( 
    SELECT replace(conrelid::regclass::text,'"','') AS "fk_table"
        ,replace(CASE WHEN pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %' THEN substring(pg_get_constraintdef(c.oid), 14, position(')' in pg_get_constraintdef(c.oid))-14) END,'"','') AS "fk_column"
        ,CASE WHEN pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %' THEN substring(pg_get_constraintdef(c.oid), position(' REFERENCES ' in pg_get_constraintdef(c.oid))+12, position('(' in substring(pg_get_constraintdef(c.oid), 14))-position(' REFERENCES ' in pg_get_constraintdef(c.oid))+1) END AS "pk_table"
        ,CASE WHEN pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %' THEN substring(pg_get_constraintdef(c.oid), position('(' in substring(pg_get_constraintdef(c.oid), 14))+14, position(')' in substring(pg_get_constraintdef(c.oid), position('(' in substring(pg_get_constraintdef(c.oid), 14))+14))-1) END AS "pk_column" 
    FROM   pg_constraint c 
    JOIN   pg_namespace n ON n.oid = c.connamespace 
    WHERE  contype IN ('f', 'p') AND 
    pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %' 
    ORDER  BY pg_get_constraintdef(c.oid), conrelid::regclass::text, contype DESC) as t
    where "pk_table" = ')"""" +table_name+"'");
    string temp = "";
    string default_constructor = "";
    string default_constructor_pointer = "";
    for (;psqlQuery->fetchNextRow();)
    {
        if (find(tables_to_generate.begin(), tables_to_generate.end(), psqlQuery->getValue("fk_table")) == tables_to_generate.end()){
            continue;
        }
        declaration += "\t\t\tbool "+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"_read_only;\n";
        constructor_destructor += "\t\t\trelatives_def[\""+psqlQuery->getValue("pk_column")+"\"][\""+psqlQuery->getValue("fk_table")+"\"]=\""+psqlQuery->getValue("fk_column")+"\";\n";
        default_constructor += "\t\t\trelatives_def[\""+psqlQuery->getValue("pk_column")+"\"][\""+psqlQuery->getValue("fk_table")+"\"]=\""+psqlQuery->getValue("fk_column")+"\";\n";
        declaration += "\t\tvector <"+psqlQuery->getValue("fk_table")+"_primitive_orm *> * "+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+";\n";
        constructor_destructor += "\t\t\t"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+" = NULL;\n";
        constructor_destructor += "\t\t\t"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"_read_only=false;\n";

        default_constructor += "\t\t\t"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+" = _"+class_name+"."+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+";\n";
        default_constructor += "\t\t\t"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"_read_only=_"+class_name+"."+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"_read_only;\n";


        default_constructor_pointer += "\t\t\trelatives_def[\""+psqlQuery->getValue("pk_column")+"\"][\""+psqlQuery->getValue("fk_table")+"\"]=\""+psqlQuery->getValue("fk_column")+"\";\n";
        default_constructor_pointer += "\t\t\t"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+" = _"+class_name+"->"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+";\n";
        default_constructor_pointer += "\t\t\t"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"_read_only=_"+class_name+"->"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"_read_only;\n";


        string file_name = psqlQuery->getValue("fk_table");
        string lower_case_file_name= "";
        for(auto& c : file_name) lower_case_file_name += tolower(c);
        includes += "#include <"+lower_case_file_name+"_primitive_orm.h>\n";
        temp += "\t\t\tif ("+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column") +"!= NULL){\n"; 

        temp += "\t\t\t\tif ("+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"_read_only)\n";   
        temp += "\t\t\t\t\tfor (auto orm :(*"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+")) delete (orm);\n";   
        temp += "\t\t\t\tdelete ("+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+");\n";   
        temp += "\t\t\t}\n";   
        getters += "vector <"+psqlQuery->getValue("fk_table")+"_primitive_orm *> * "+class_name+"::get_"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"(bool _read_only){\n";
        getters += "\t\tif ("+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column") +"== NULL) {\n";
        getters += "\t\t\t"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"_read_only = _read_only;\n";
        getters += "\t\t\t"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+" = new vector <"+psqlQuery->getValue("fk_table")+"_primitive_orm *> ();\n";
        getters += "\t\t\t"+psqlQuery->getValue("fk_table")+"_primitive_orm_iterator * i = new "+psqlQuery->getValue("fk_table")+"_primitive_orm_iterator(data_source_name);\n";

        getters += "\t\t\ti->filter (ANDOperator(new UnaryOperator(\""+psqlQuery->getValue("fk_column")+"\",eq,get_"+primary_key+"())));\n";
        getters += "\t\t\ti->execute();\n";
        getters += "\t\t\t"+psqlQuery->getValue("fk_table")+"_primitive_orm * orm = NULL;\n";
//        getters += "\t\t\t"+psqlQuery->getValue("fk_table")+"_primitive_orm * orm = new "+psqlQuery->getValue("fk_table")+"_primitive_orm();\n";
        getters += "\t\t\tdo {\n";
		getters += "\t\t\t\torm = i->next(_read_only);\n";
		getters += "\t\t\t\tif (orm!= NULL) "+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column") +"->push_back(orm);\n";
		getters += "\t\t\t} while (orm != NULL);\n";
        getters += "\t\t\tdelete(i);\n";
        getters += "\t\t\t}\n";
        getters += "\t\t\treturn "+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column") +";\n}\n";
        getters_def +=  "\t\tvector <"+psqlQuery->getValue("fk_table")+"_primitive_orm *> * get_"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"(bool _read_only=false);\n";
    }
    delete (psqlQuery);
    constructor_destructor +="\t\t}\n";

    // constructor_destructor += "\t\tvoid "+class_name+"::operator = (const "+class_name+" & _"+class_name+"){\n";
    // constructor_destructor += "\t\t\t *((PSQLAbstractORM *)this) = _"+class_name+";\n";
    // constructor_destructor += "\t\t\tthis->orm_"+primary_key+"= _"+class_name+".orm_"+primary_key+";\n";
    // constructor_destructor += "\t\t\tthis->table_index=_"+class_name+".table_index;\n";
    // constructor_destructor += "\t\t\tcached=_"+class_name+".cached;\n";

    // constructor_destructor += default_constructor;
    // for (int i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    // {
    //     if (databaseColumnFactory.find(columns_definition["udt_name"][i]) != databaseColumnFactory.end()) {
    //         constructor_destructor += "\t\t\torm_"+columns_definition["column_name"][i];
    //         constructor_destructor += " = _"+class_name+".orm_" +columns_definition["column_name"][i]+ ";\n";            
    //     }
    // }
    // constructor_destructor += "\t\t\tsetIdentifier(-1);\n";
    // constructor_destructor += "\t\t\tif (cached) this->addToCache();\n";


    // constructor_destructor += "\t\t}\n";

    // constructor_destructor += "\t\tvoid "+class_name+"::operator = (const "+class_name+" * _"+class_name+"){\n";
    // constructor_destructor += "\t\t\t *((PSQLAbstractORM *)this) = _"+class_name+";\n";
    // constructor_destructor += "\t\t\tthis->orm_"+primary_key+"= _"+class_name+"->orm_"+primary_key+";\n";
    // constructor_destructor += "\t\t\tthis->table_index=_"+class_name+"->table_index;\n";
    // constructor_destructor += "\t\t\tcached=_"+class_name+"->cached;\n";

    // constructor_destructor += default_constructor_pointer;
    // for (int i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    // {
    //     if (databaseColumnFactory.find(columns_definition["udt_name"][i]) != databaseColumnFactory.end()) {
    //         constructor_destructor += "\t\t\torm_"+columns_definition["column_name"][i];
    //         constructor_destructor += " = _"+class_name+"->orm_" +columns_definition["column_name"][i]+ ";\n";            
    //     }
    // }
    // constructor_destructor += "\t\t\tloaded=_"+class_name+"->loaded;\n";
    // constructor_destructor += "\t\t\tupdate_flag=_"+class_name+"->update_flag;\n";

    // constructor_destructor += "\t\t}\n";


    constructor_destructor += "\t\t"+class_name+"::"+class_name+"(const "+class_name+" & _"+class_name+"): PSQLAbstractORM(_"+class_name+"){\n";
    constructor_destructor += "\t\t\t (*this) = _"+class_name+";\n";
    // constructor_destructor += "\t\t\tthis->orm_"+primary_key+"= _"+class_name+".orm_"+primary_key+";\n";
    // constructor_destructor += "\t\t\tthis->table_index=_"+class_name+".table_index;\n";
    // constructor_destructor += "\t\t\tcached=_"+class_name+".cached;\n";
    // constructor_destructor += "\t\t\tif (cached) this->addToCache();\n";

    // constructor_destructor += default_constructor;
    constructor_destructor += "\t\t}\n";


    constructor_destructor += "\t\t "+class_name+"::~"+class_name+"(){\n";
    constructor_destructor += temp+"}\n";

    constructor_destructor_def = "\t\t"+class_name+"(string data_source_name,bool add_to_cache=false, bool orm_transactional=true,int _enforced_partition_number=-1, vector <string> _field_clear_mask = {},bool _seeder_readonly=false);\n";
    constructor_destructor_def += "\t\t"+class_name+"(const "+class_name+" & _"+class_name+");\n";
    // constructor_destructor_def += "\t\tvirtual void operator =(const "+class_name+" & _"+class_name+");\n";
    // constructor_destructor_def += "\t\tvirtual void operator =(const "+class_name+" * _"+class_name+");\n";
    constructor_destructor_def += "\t\t virtual ~"+class_name+"();\n";

}
void PSQLPrimitiveORMGenerator::generateAddToCache(string class_name)
{
    extra_methods_def += "\t\tvoid addToCache ();\n";
    extra_methods += "\t\tvoid "+class_name+"::addToCache (){\n";
    extra_methods += "\t\t\t"+class_name+" * orm = ("+class_name+" *) psqlController.addToORMCache(\""+class_name+"\",this,data_source_name);\n";
    extra_methods += "\t\t\tif (orm!= NULL) {\n";
    extra_methods += "\t\t\t\t(*this) = (orm);\n";
    extra_methods += "\t\t\t\tdelete(orm);\n";
    extra_methods += "\t\t\t}\n";
    extra_methods += "\t\t}\n";

}
void PSQLPrimitiveORMGenerator::generateIsUpdated(string class_name,bool is_view)
{
    extra_methods_def += "\t\tbool isUpdated ();\n";
    if ( ! is_view)
    {
        extra_methods += "\t\tbool "+class_name+"::isUpdated (){\n";
        extra_methods += "\t\t\tcommitUpdateReferences();\n";
        extra_methods += "\t\t\tresolveReferences();\n";
        extra_methods += "\t\t\treturn update_flag.any();\n";
    }
    else extra_methods += "\t\t\treturn false;\n";
    extra_methods += "\t\t}\n";
}


void PSQLPrimitiveORMGenerator::generateStaticFetch(string class_name)
{
    extra_methods_def += "\t\tstatic ORMVector <"+class_name+"> fetch(string _data_source_name, const Expression & e, bool _read_only = true);\n";
	extra_methods +="\t\tORMVector <"+class_name+"> "+class_name+"::fetch(string _data_source_name, const Expression & e, bool _read_only){\n";
    extra_methods +="\t\t\tORMVector <"+class_name+"> ormVector;\n";
    // extra_methods +="\t\t\tbool _read_only = true;\n";
    extra_methods +="\t\t\t"+class_name+"_iterator * i = new "+class_name+"_iterator(_data_source_name);\n";
    extra_methods +="\t\t\ti->filter (e);\n";
    extra_methods +="\t\t\ti->execute();\n";
    extra_methods +="\t\t\t"+class_name+" * orm = NULL;\n";
    extra_methods +="\t\t\tdo {\n";
    extra_methods +="\t\t\t\torm = i->next(_read_only);\n";
    extra_methods +="\t\t\t\tif (orm!= NULL) ormVector.push_back(orm);\n";
    extra_methods +="\t\t\t} while (orm != NULL);\n";
    extra_methods +="\t\t\tdelete(i);\n";
    extra_methods +="\t\t\treturn ormVector;\n";
    extra_methods +="\t\t}\n";
}

void PSQLPrimitiveORMGenerator::generateResolveReferences(string class_name,string table_name,map<string, vector<string>> columns_definition)
{
    extra_methods_def += "\t\tvoid resolveReferences ();\n";
    extra_methods += "\t\tvoid "+class_name+"::resolveReferences (){\n";

    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        string db_field_name = columns_definition["column_name"][i];
        string orm_field_name = "orm_"+columns_definition["column_name"][i];
        bool string_flag = false;
        for ( int j = 0 ; PSQLText::get_native_type(j) != "" ; j ++)
            if ( PSQLText::get_native_type(j) == columns_definition["udt_name"][i]) string_flag=true;

        bool json_flag = false;
        for ( int j = 0 ; PSQLJson::get_native_type(j) != "" ; j ++)
            if ( PSQLJson::get_native_type(j) == columns_definition["udt_name"][i]) json_flag=true;

        if ( db_field_name != primary_key && ! json_flag && !string_flag )
        {
            extra_methods += "\t\t\tif(reference_values.find(\""+db_field_name+"\") != reference_values.end())\n ";
            extra_methods += "\t\t\t\tset_"+db_field_name+"( reference_values[\""+db_field_name+"\"]);\n";

        }
     }
    extra_methods += "\t\t}\n";
}

void PSQLPrimitiveORMGenerator::generateUpdateQuery(string class_name,string table_name,map<string, vector<string>> columns_definition,bool is_view)
{

    extra_methods_def += "\t\tbool update (PSQLConnection * _psqlConnection=NULL);\n";
    extra_methods += "\t\tbool "+class_name+"::update (PSQLConnection * _psqlConnection){\n";
    if (!is_view)
    {
    extra_methods += "\t\t\tcommitUpdateReferences(_psqlConnection);\n";
    extra_methods += "\t\t\tresolveReferences();\n";
    extra_methods += "\t\t\tstring update_string = \"\";\n";
    for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        string db_field_name = columns_definition["column_name"][i];
        string orm_field_name = "orm_"+columns_definition["column_name"][i];
        bool string_flag = false;
        for ( int j = 0 ; PSQLText::get_native_type(j) != "" ; j ++)
            if ( PSQLText::get_native_type(j) == columns_definition["udt_name"][i]) string_flag=true;

        bool json_flag = false;
        for ( int j = 0 ; PSQLJson::get_native_type(j) != "" ; j ++)
            if ( PSQLJson::get_native_type(j) == columns_definition["udt_name"][i]) json_flag=true;

        if ( db_field_name != primary_key)
        {
            extra_methods += "\t\t\tif (update_flag.test("+std::to_string(i)+") && (!field_clear_mask_flag.any() || field_clear_mask_flag.test("+std::to_string(i)+"))) {\n";
            extra_methods += "\t\t\t\tif (update_string != \"\")  update_string += \",\";\n";
            if (string_flag )
                extra_methods += "\t\t\t\t\tupdate_string += \""+db_field_name+"='\"+"+orm_field_name+"+\"'\";\n";
            else if (json_flag )
                extra_methods += "\t\t\t\t\tupdate_string += \""+db_field_name+"='\"+"+orm_field_name+".dump()+\"'\";\n";                
            else extra_methods += "\t\t\t\tupdate_string += \""+db_field_name+"='\"+std::to_string("+orm_field_name+")+\"'\";\n";
            extra_methods += "\t\t\t}\n";
            extra_methods += "\t\t\telse\n";
            extra_methods += "\t\t\t{\n";
            extra_methods += "\t\t\t\tif(update_default_values.find(\""+db_field_name+"\") != update_default_values.end())\n ";
            extra_methods += "\t\t\t\t{\n";
            extra_methods += "\t\t\t\t\tif(update_default_values[\""+db_field_name+"\"].second )\n";
            extra_methods += "\t\t\t\t\t\tupdate_string += \""+db_field_name+"=\"+update_default_values[\""+db_field_name+"\"].first;\n";
            extra_methods += "\t\t\t\t\telse update_string += \""+db_field_name+"='\"+update_default_values[\""+db_field_name+"\"].first+\"'\";\n";                
            extra_methods += "\t\t\t\t}\n";

            extra_methods += "\t\t\t}\n";
        }
    }

    extra_methods += "\t\t\tbool return_flag=true;\n";
    extra_methods += "\t\t\tif (update_string != \"\")  {\n";
    extra_methods += "\t\t\t\tupdate_string = \"update "+table_name+" set \"+update_string+\" where "+primary_key+"= '\"+std::to_string(orm_"+primary_key+")+\"'\";\n";
    extra_methods += "\t\t\t\tPSQLConnection * psqlConnection = _psqlConnection;\n";
    extra_methods += "\t\t\t\tif (_psqlConnection == NULL )\n";
    extra_methods += "\t\t\t\t\tpsqlConnection = psqlController.getPSQLConnection(data_source_name);\n";
    extra_methods += "\t\t\t\treturn_flag=psqlConnection->executeUpdateQuery(update_string);\n";
    extra_methods += "\t\t\t\tupdate_flag.reset();\n";
    extra_methods += "\t\t\t\tif (_psqlConnection == NULL )\n";
    extra_methods += "\t\t\t\t\tpsqlController.releaseConnection(data_source_name,psqlConnection);\n";
    extra_methods += "\t\t\t}\n";
    extra_methods += "\t\t\telse return return_flag;\n";


    extra_methods += "\t\t\treturn return_flag;\n";
    }
    else extra_methods += "\t\t\treturn true;\n";
    extra_methods += "\t\t}\n";
}


void PSQLPrimitiveORMGenerator::generateInsertQuery(string class_name,string table_name,map<string, vector<string>> columns_definition,bool is_view)
{

    extra_methods_def += "\t\tlong insert (PSQLConnection * _psqlConnection=NULL);\n";
    extra_methods += "\t\tlong "+class_name+"::insert (PSQLConnection * _psqlConnection){\n";
    if (!is_view)
    {
        extra_methods += "\t\t\t if (inserted) return orm_"+primary_key+";\n";

        extra_methods += "\t\t\tcommitAddReferences(_psqlConnection);\n";
        extra_methods += "\t\t\tresolveReferences();\n";
        // extra_methods += "\t\t\tstring insert_string = \"\";\n";
        string columns_string = "";
        string values_string = "";
        for (size_t   i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
        {
            string db_field_name = columns_definition["column_name"][i];
            string orm_field_name = "orm_"+columns_definition["column_name"][i];
            bool string_flag = false;
            for ( int j = 0 ; PSQLText::get_native_type(j) != "" ; j ++)
                if ( PSQLText::get_native_type(j) == columns_definition["udt_name"][i]) string_flag=true;

            bool json_flag = false;
            for ( int j = 0 ; PSQLJson::get_native_type(j) != "" ; j ++)
                if ( PSQLJson::get_native_type(j) == columns_definition["udt_name"][i]) json_flag=true;
            
            bool ts_flag = false;
            if ( columns_definition["udt_name"][i] == "timestamptz") ts_flag=true;

            if ( db_field_name != primary_key)
            {
                if ( columns_string != "") columns_string+= ",";
                if ( values_string != "") values_string+= "+string(\",\")+";

                values_string+= "((insert_default_values.find(\""+db_field_name+"\") != insert_default_values.end())? ";

                values_string+="((insert_default_values[\""+db_field_name+"\"].second)?insert_default_values[\""+db_field_name+"\"].first:string(\"'\")+insert_default_values[\""+db_field_name+"\"].first+string(\"'\"))";

                values_string+= ":";


                columns_string += db_field_name;

                if (ts_flag )
                {
                    if(columns_definition["is_nullable"][i] == "NO") values_string += "(("+orm_field_name+" == \"\") ? \"now()\" :string(\"'\")+"+orm_field_name+"+string(\"'\"))";
                    else values_string += "(("+orm_field_name+" == \"\") ? \"null\" :string(\"'\")+"+orm_field_name+"+string(\"'\"))";
                }
                else{
                    values_string += "((update_flag.test("+std::to_string(i)+") && ! null_flag.test("+std::to_string(i)+"))?";
                    if (string_flag )
                        values_string += "string(\"'\")+"+orm_field_name+"+string(\"'\")";
                    else if (json_flag )
                        values_string += "string(\"'\")+"+orm_field_name+".dump()+string(\"'\")";
                    else values_string += "string(\"'\")+std::to_string("+orm_field_name+")+string(\"'\")";
                    values_string += ":\"null\")";
                }
                values_string += ")";
            }
        }

        if ( columns_string !="")
        {
            extra_methods += "\t\t\tif (field_clear_mask_flag.any()) {inserted = false;return -1;}\n";
            extra_methods += "\t\t\tstring insert_string = \"insert into "+table_name+" ("+columns_string+") values (\"+"+values_string+"+\") returning id\";\n";
            extra_methods += "\t\t\tPSQLConnection * psqlConnection = _psqlConnection;\n";
            extra_methods += "\t\t\tif (psqlConnection == NULL )\n";
            extra_methods += "\t\t\t\tpsqlConnection = psqlController.getPSQLConnection(data_source_name);\n";
            extra_methods += "\t\t\torm_"+primary_key+"=psqlConnection->executeInsertQuery(insert_string);\n";
            extra_methods += "\t\t\tif (_psqlConnection == NULL )\n";
            extra_methods += "\t\t\t\tpsqlController.releaseConnection(data_source_name,psqlConnection);\n";
            extra_methods += "\t\t\tupdate_flag.reset();\n";
        } 
        extra_methods += "\t\t\tinserted = true;\n ";
        extra_methods += "\t\t\treturn orm_"+primary_key+";\n";
    } else extra_methods += "\t\t return true;}\n";
    extra_methods += "\t\t}\n";
}

void PSQLPrimitiveORMGenerator::generate(string table_name,string table_index, vector<string> &tables_to_generate, bool is_views)
{
    AbstractDBQuery *psqlQuery = psqlConnection->executeQuery("select table_name,column_name,data_type,numeric_precision,numeric_precision_radix,numeric_scale,is_nullable,is_generated,identity_generation,is_identity,column_default,identity_increment,udt_name from information_schema.COLUMNS where table_name='"+table_name+"'");
    string class_name = table_name+"_primitive_orm";
    string include_file = table_name+"_primitive_orm";
    string lower_case_file_name= "";
    for(auto& c : include_file) lower_case_file_name += tolower(c);
    include_file= lower_case_file_name;
    string query_iterator_class_name = table_name+"_primitive_orm_iterator";
    string class_name_upper = table_name+"_primitive_orm_h";
    std::transform(class_name_upper.begin(), class_name_upper.end(), class_name_upper.begin(), ::toupper);
    extra_methods = "";
    extra_methods_def = "";

    map<string, vector<string>> results  = psqlQuery->getResultAsString();
    get_primary_key(table_name,is_views);
    generateFieldsMap(class_name,table_index,results);

    if (primary_key != "" )
    {
        generateDecl_Setters_Getters(class_name,results);
        generateFromString(class_name,table_name,table_index,results);
        generateAssignResults(class_name,table_index,results,is_views);
        generateAssignmentOperator(class_name,table_index,results);
        generateGetIdentifier(class_name);
        generateCloner(class_name);
        generateExternDSOEntryPoint(class_name,table_name);
        generateConstructorAndDestructor(class_name,table_name,table_index,results,tables_to_generate);
        generateUpdateQuery(class_name,table_name,results,is_views);
        generateInsertQuery(class_name,table_name,results,is_views);
        generateSerializer(class_name,table_name,results);
        generateDeserializer(class_name,table_name,results);
        generateEqualToOperator(class_name,table_name,results);
        generateAddToCache(class_name);
        generateIsUpdated(class_name,is_views);
        generateResolveReferences(class_name,table_name,results);
        generateStaticFetch(class_name);
        snprintf (h_file,MAX_SOURCE_FILE_SIZE,template_h,
        class_name_upper.c_str(),class_name_upper.c_str(),includes.c_str(),
        class_name.c_str(),"",declaration.c_str(),(setters_def+getters_def+extra_methods_def+constructor_destructor_def).c_str(),
        query_iterator_class_name.c_str(),query_iterator_class_name.c_str(),class_name.c_str(),
        query_iterator_class_name.c_str(),class_name.c_str(),class_name.c_str(),class_name.c_str(),class_name.c_str(),query_iterator_class_name.c_str());
        snprintf (cpp_file,MAX_SOURCE_FILE_SIZE,template_cpp,include_file.c_str(),(setters+getters+extra_methods+constructor_destructor+extern_entry_point).c_str()
        ,query_iterator_class_name.c_str(),query_iterator_class_name.c_str(),table_name.c_str(),class_name.c_str(),class_name.c_str()
        ,class_name.c_str(),query_iterator_class_name.c_str()
        ,class_name.c_str(),query_iterator_class_name.c_str(),class_name.c_str(),class_name.c_str(),class_name.c_str(),class_name.c_str(),class_name.c_str()
        ,class_name.c_str(),query_iterator_class_name.c_str()
        ,query_iterator_class_name.c_str(),query_iterator_class_name.c_str(),class_name.c_str(),class_name.c_str(),class_name.c_str()
        ,query_iterator_class_name.c_str(),class_name.c_str()
        ,query_iterator_class_name.c_str(),query_iterator_class_name.c_str(),query_iterator_class_name.c_str());
        write_headers_and_sources(class_name);
    }
    delete (psqlQuery);
}
void PSQLPrimitiveORMGenerator::compile(string table_name)
{
    string class_name = table_name+"_primitive_orm";
    string sys_cmd = "g++ -shared -fPIC -std=c++20 ./factory/db_primitive_orm/sources/"+class_name+".cpp $(ls ./objects | grep -v orm_c++| grep cpp |grep -v orm.cpp.o|awk '{print \"./objects/\" $1}') -o ./dso/"+class_name+".so -I./headers/postgres/ -I ./ -I./headers/abstract -I /usr/local/Cellar/libpq/16.0/include/ -I ./factory/db_primitive_orm/headers/ -L/usr/local/Cellar/libpq/16.0/lib -lpthread -ldl -lpq";
    cout << sys_cmd << endl;
    system (sys_cmd.c_str());
}


void PSQLPrimitiveORMGenerator::createFoldersIfNotExist(const string& path) {
    std::filesystem::path dirPath(path);

    if (!std::filesystem::exists(dirPath)) {
        if (std::filesystem::create_directories(dirPath)) {
            cout << "Successfully created directories: " << path << endl;
        } else {
            cerr << "Failed to create directories: " << path << endl;
        }
    } else {
        cout << "Directory already exists: " << path << endl;
    }
}


PSQLPrimitiveORMGenerator::~PSQLPrimitiveORMGenerator()
{
    for (auto psql_type: databaseColumnFactory) 
        delete (psql_type.second);
    psqlController.releaseConnection (this->datasource,psqlConnection);
    if (template_h != NULL) free (template_h);
    if (template_cpp != NULL) free (template_cpp);
    free (h_file);
    free (cpp_file);
    
}