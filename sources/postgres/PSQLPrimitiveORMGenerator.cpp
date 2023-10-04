#include <PSQLPrimitiveORMGenerator.h>

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
    string h_file_name = SOURCES_H_FILENAME +class_name+".h";
    string cpp_file_name = SOURCES_CPP_FILENAME +class_name+".cpp";

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


PSQLPrimitiveORMGenerator::PSQLPrimitiveORMGenerator()
{
    psqlConnection = new PSQLConnection ("localhost",5432,"django_ostaz_15082023_old","postgres","postgres");
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
void PSQLPrimitiveORMGenerator::get_primary_key(string table_name){
    primary_key = "";
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
    for (int i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        if (databaseColumnFactory.find(columns_definition["udt_name"][i]) != databaseColumnFactory.end()) {
            AbstractDatabaseColumn * abstractDatabaseColumn = databaseColumnFactory[columns_definition["udt_name"][i]]->clone(columns_definition["column_name"][i]);
            declaration += abstractDatabaseColumn->genDeclaration();
            setters += abstractDatabaseColumn->genSetter(class_name);
            getters += abstractDatabaseColumn->genGetter(class_name);
            setters_def += abstractDatabaseColumn->genSetterDef();
            getters_def += abstractDatabaseColumn->genGetterDef();
            delete(abstractDatabaseColumn);
        }
        else cout << "TYPE: " << columns_definition["udt_name"][i] << " - " << columns_definition["numeric_precision"][i] << " - " << columns_definition["numeric_precision_radix"][i] << " - "<< columns_definition["numeric_scale"][i] << endl;
    }
}

void PSQLPrimitiveORMGenerator::generateFromString (string class_name,string table_name,map<string, vector<string>> columns_definition)
{
    extra_methods_def += "\t\tstring getFromString ();\n";
    extra_methods += "\t\tstring "+class_name+"::getFromString (){\n\t\t\treturn \"";
    for (int i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        if ( i > 0 )  extra_methods  += ",";
        extra_methods += "\\\""+table_name+"\\\".\\\""+columns_definition["column_name"][i]+"\\\" as \\\""+table_name+"_"+columns_definition["column_name"][i]+"\\\"";
    }
    extra_methods += "\";\n\t\t}\n";
}

void PSQLPrimitiveORMGenerator::generateAssignResults (string class_name,string table_name,map<string, vector<string>> columns_definition)
{
    extra_methods_def += "\t\tvoid assignResults (AbstractDBQuery * psqlQuery);\n";
    extra_methods += "\t\tvoid "+class_name+"::assignResults (AbstractDBQuery * psqlQuery){\n";
    // extra_methods += "\t\t\tpsqlQuery->fetchNextRow();\n";
    for (int i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        if (databaseColumnFactory.find(columns_definition["udt_name"][i]) != databaseColumnFactory.end()) {
            AbstractDatabaseColumn * abstractDatabaseColumn = databaseColumnFactory[columns_definition["udt_name"][i]]->clone(columns_definition["column_name"][i]);
            extra_methods += "\t\t\torm_"+columns_definition["column_name"][i];
            if ( columns_definition["udt_name"][i] == "jsonb")
                extra_methods += " = " +abstractDatabaseColumn->genFieldConversion("psqlQuery->getJSONValue(\"" +table_name+"_"+ columns_definition["column_name"][i]+"\")")+ ";\n";            
            else extra_methods += " = " +abstractDatabaseColumn->genFieldConversion("psqlQuery->getValue(\"" +table_name+"_"+ columns_definition["column_name"][i]+"\")")+ ";\n";
            delete(abstractDatabaseColumn);
        }
    }
    extra_methods += "\t\t}\n";
}
void PSQLPrimitiveORMGenerator::generateGetIdentifier(string class_name)
{
    extra_methods_def += "\t\tlong getIdentifier ();\n";
    extra_methods += "\t\tlong "+class_name+"::getIdentifier (){\n";
    extra_methods += "\t\t\treturn orm_"+primary_key+";\n";
    extra_methods += "\t\t}\n";
}
void PSQLPrimitiveORMGenerator::generateCloner(string class_name)
{
    extra_methods_def += "\t\tPSQLAbstractORM * clone ();\n";
    extra_methods += "\t\tPSQLAbstractORM * "+class_name+"::clone (){\n";
    extra_methods += "\t\t\treturn new "+class_name+"();\n";
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

void PSQLPrimitiveORMGenerator::generateConstructorAndDestructor(string class_name,string table_name)
{
    includes = "";
    constructor_destructor = "\t\t"+class_name+"::"+class_name+"():PSQLAbstractORM(\""+table_name+"\",\""+primary_key+"\"){\n";
    // constructor_destructor +="\t\t\tpsqlQuery = psqlConnection->executeQuery(\"select \"+this->getFromString()+\" from "+table_name+"\");\n";
    AbstractDBQuery *psqlQuery = psqlConnection->executeQuery(R""""(select * from ( 
    SELECT replace(conrelid::regclass::text,'"','') AS "fk_table"
        ,CASE WHEN pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %' THEN substring(pg_get_constraintdef(c.oid), 14, position(')' in pg_get_constraintdef(c.oid))-14) END AS "fk_column"
        ,CASE WHEN pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %' THEN substring(pg_get_constraintdef(c.oid), position(' REFERENCES ' in pg_get_constraintdef(c.oid))+12, position('(' in substring(pg_get_constraintdef(c.oid), 14))-position(' REFERENCES ' in pg_get_constraintdef(c.oid))+1) END AS "pk_table"
        ,CASE WHEN pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %' THEN substring(pg_get_constraintdef(c.oid), position('(' in substring(pg_get_constraintdef(c.oid), 14))+14, position(')' in substring(pg_get_constraintdef(c.oid), position('(' in substring(pg_get_constraintdef(c.oid), 14))+14))-1) END AS "pk_column" 
    FROM   pg_constraint c 
    JOIN   pg_namespace n ON n.oid = c.connamespace 
    WHERE  contype IN ('f', 'p') AND 
    pg_get_constraintdef(c.oid) LIKE 'FOREIGN KEY %' 
    ORDER  BY pg_get_constraintdef(c.oid), conrelid::regclass::text, contype DESC) as t
    where "pk_table" = ')"""" +table_name+"'");
    string temp = "";
    for (;psqlQuery->fetchNextRow();)
    {
        constructor_destructor += "\t\trelatives_def[\""+psqlQuery->getValue("pk_column")+"\"][\""+psqlQuery->getValue("fk_table")+"\"]=\""+psqlQuery->getValue("fk_column")+"\";\n";
        declaration += "\t\tvector <"+psqlQuery->getValue("fk_table")+"_primitive_orm *> * "+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+";\n";
        constructor_destructor += "\t\t"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+" = NULL;\n";
        includes += "#include <"+psqlQuery->getValue("fk_table")+"_primitive_orm.h>\n";
        temp += "\t\tif ("+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column") +"!= NULL)delete ("+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+");\n";   
        getters += "vector <"+psqlQuery->getValue("fk_table")+"_primitive_orm *> * "+class_name+"::get_"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"(){\n";
        getters += "\t\tif ("+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column") +"== NULL) {\n";
        getters += "\t\t\t"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+" = new vector <"+psqlQuery->getValue("fk_table")+"_primitive_orm *> ();\n";
        getters += "\t\t\t"+psqlQuery->getValue("fk_table")+"_primitive_orm_iterator * i = new "+psqlQuery->getValue("fk_table")+"_primitive_orm_iterator(\"main\");\n";

        getters += "\t\t\ti->filter (ANDOperator(new UnaryOperator(\""+psqlQuery->getValue("fk_column")+"\",eq,get_"+primary_key+"())));\n";
        getters += "\t\t\ti->execute();\n";
        getters += "\t\t\t"+psqlQuery->getValue("fk_table")+"_primitive_orm * orm = new "+psqlQuery->getValue("fk_table")+"_primitive_orm();\n";
        getters += "\t\t\tdo {\n";
		getters += "\t\t\t\torm = i->next();\n";
		getters += "\t\t\t\tif (orm!= NULL) "+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column") +"->push_back(orm);\n";
		getters += "\t\t\t} while (orm != NULL);\n";
        getters += "\t\t\t}\n";
        getters += "\t\t\treturn "+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column") +";\n}\n";
        getters_def +=  "\t\tvector <"+psqlQuery->getValue("fk_table")+"_primitive_orm *> * get_"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"();\n";
    }
    delete (psqlQuery);
    constructor_destructor +="\t\t}\n";
    constructor_destructor += "\t\t "+class_name+"::~"+class_name+"(){\n";
    constructor_destructor += temp+"}\n";

    constructor_destructor_def = "\t\t"+class_name+"();\n";
    constructor_destructor_def += "\t\t virtual ~"+class_name+"();\n";

}

void PSQLPrimitiveORMGenerator::generate(string table_name)
{
    AbstractDBQuery *psqlQuery = psqlConnection->executeQuery("select table_name,column_name,data_type,numeric_precision,numeric_precision_radix,numeric_scale,is_nullable,is_generated,identity_generation,is_identity,column_default,identity_increment,udt_name from information_schema.COLUMNS where table_name='"+table_name+"'");
    string class_name = table_name+"_primitive_orm";
    string query_iterator_class_name = table_name+"_primitive_orm_iterator";
    string class_name_upper = table_name+"_primitive_orm_h";
    std::transform(class_name_upper.begin(), class_name_upper.end(), class_name_upper.begin(), ::toupper);
    extra_methods = "";
    extra_methods_def = "";

    map<string, vector<string>> results  = psqlQuery->getResultAsString();
    get_primary_key(table_name);
    if (primary_key != "" )
    {
        generateDecl_Setters_Getters(class_name,results);
        generateFromString(class_name,table_name,results);
        generateAssignResults(class_name,table_name,results);
        generateGetIdentifier(class_name);
        generateCloner(class_name);
        generateExternDSOEntryPoint(class_name,table_name);
        generateConstructorAndDestructor(class_name,table_name);
        snprintf (h_file,MAX_SOURCE_FILE_SIZE,template_h,
        class_name_upper.c_str(),class_name_upper.c_str(),includes.c_str(),
        class_name.c_str(),"",declaration.c_str(),(setters_def+getters_def+extra_methods_def+constructor_destructor_def).c_str(),
        query_iterator_class_name.c_str(),class_name.c_str(),
        query_iterator_class_name.c_str(),class_name.c_str(),class_name.c_str(),class_name.c_str(),class_name.c_str(),query_iterator_class_name.c_str());
        snprintf (cpp_file,MAX_SOURCE_FILE_SIZE,template_cpp,class_name.c_str(),(setters+getters+extra_methods+constructor_destructor+extern_entry_point).c_str()
        ,query_iterator_class_name.c_str(),query_iterator_class_name.c_str(),table_name.c_str(),class_name.c_str()
        ,class_name.c_str(),query_iterator_class_name.c_str()
        ,class_name.c_str(),query_iterator_class_name.c_str(),class_name.c_str(),class_name.c_str()
        ,class_name.c_str(),query_iterator_class_name.c_str()
        ,query_iterator_class_name.c_str(),class_name.c_str(),class_name.c_str(),class_name.c_str()
        ,query_iterator_class_name.c_str(),class_name.c_str()
        ,query_iterator_class_name.c_str(),query_iterator_class_name.c_str());
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

PSQLPrimitiveORMGenerator::~PSQLPrimitiveORMGenerator()
{
    for (auto psql_type: databaseColumnFactory) 
        delete (psql_type.second);
    delete (psqlConnection);
    if (template_h != NULL) free (template_h);
    if (template_cpp != NULL) free (template_cpp);
    free (h_file);
    free (cpp_file);
    
}