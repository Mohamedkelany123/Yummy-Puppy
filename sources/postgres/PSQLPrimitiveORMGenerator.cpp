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
    databaseColumnFactory[PSQLInt4::get_native_type()] = new PSQLInt4();
    databaseColumnFactory[PSQLInt8::get_native_type()] = new PSQLInt8();
    template_h = NULL;
    template_cpp = NULL;
    h_file = (char *) calloc (MAX_SOURCE_FILE_SIZE,sizeof(char));
    cpp_file = (char *) calloc (MAX_SOURCE_FILE_SIZE,sizeof(char));
    fetch_templates();
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
    }
}

void PSQLPrimitiveORMGenerator::generateFromString (string class_name,map<string, vector<string>> columns_definition)
{
    extra_methods_def += "\t\tstring getFromString ();\n";
    extra_methods += "\t\tstring "+class_name+"::getFromString (){\n\t\t\treturn \"";
    for (int i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        if ( i > 0 )  extra_methods  += ",";
        extra_methods += "\\\""+columns_definition["column_name"][i]+"\\\"";
    }
    extra_methods += "\";\n\t\t}\n";
}

void PSQLPrimitiveORMGenerator::generateAssignResults (string class_name,map<string, vector<string>> columns_definition)
{
    extra_methods_def += "\t\tvoid assignResults ();\n";
    extra_methods += "\t\tvoid "+class_name+"::assignResults (){\n";
    extra_methods += "\t\t\tpsqlQuery->fetchNextRow();\n";
    for (int i  = 0 ; i  < columns_definition["column_name"].size(); i++) 
    {
        if (databaseColumnFactory.find(columns_definition["udt_name"][i]) != databaseColumnFactory.end()) {
            AbstractDatabaseColumn * abstractDatabaseColumn = databaseColumnFactory[columns_definition["udt_name"][i]]->clone(columns_definition["column_name"][i]);
            extra_methods += "\t\t\t"+columns_definition["column_name"][i];
            extra_methods += " = " +abstractDatabaseColumn->genFieldConversion("psqlQuery->getValue(\"" + columns_definition["column_name"][i]+"\")")+ ";\n";
            delete(abstractDatabaseColumn);
        }
    }
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
    constructor_destructor = "\t\t"+class_name+"::"+class_name+"():PSQLAbstractORM(\""+table_name+"\"){\n";
    constructor_destructor +="\t\t\tpsqlQuery = psqlConnection->executeQuery(\"select \"+this->getFromString()+\" from "+table_name+"\");\n";
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
        getters += "vector <"+psqlQuery->getValue("fk_table")+"_primitive_orm *> * "+class_name+"::get_"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"(){ \n";
        getters += "\t\tif ("+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column") +"== NULL)\n\t\t\t "+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+" = new vector <"+psqlQuery->getValue("fk_table")+"_primitive_orm *> ();\n";
        getters += "\t\treturn "+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+";\n}\n";
        getters_def +=  "vector <"+psqlQuery->getValue("fk_table")+"_primitive_orm *> * get_"+psqlQuery->getValue("fk_table") +"_"+psqlQuery->getValue("fk_column")+"();\n";
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
    string class_name_upper = table_name+"_primitive_orm_h";
    std::transform(class_name_upper.begin(), class_name_upper.end(), class_name_upper.begin(), ::toupper);
    extra_methods = "";
    extra_methods_def = "";

    map<string, vector<string>> results  = psqlQuery->getResultAsString();
    generateDecl_Setters_Getters(class_name,results);
    generateFromString(class_name,results);
    generateAssignResults(class_name,results);
    generateExternDSOEntryPoint(class_name,table_name);
    generateConstructorAndDestructor(class_name,table_name);
    snprintf (h_file,MAX_SOURCE_FILE_SIZE,template_h,class_name_upper.c_str(),class_name_upper.c_str(),includes.c_str(),class_name.c_str(),"",declaration.c_str(),(setters_def+getters_def+extra_methods_def+constructor_destructor_def).c_str());
    snprintf (cpp_file,MAX_SOURCE_FILE_SIZE,template_cpp,class_name.c_str(),(setters+getters+extra_methods+constructor_destructor+extern_entry_point).c_str());

    write_headers_and_sources(class_name);
    delete (psqlQuery);
}
void PSQLPrimitiveORMGenerator::compile(string table_name)
{
    string class_name = table_name+"_primitive_orm";
    string sys_cmd = "g++ -shared -fPIC -std=c++20 ./factory/db_primitive_orm/sources/"+class_name+".cpp $(ls ./objects | grep -v orm_c++| grep -v orm.cpp.o|awk '{print \"./objects/\" $1}') -o ./dso/"+class_name+".so -I./headers/postgres/ -I ./ -I./headers/abstract -I /usr/local/Cellar/libpq/16.0/include/ -I ./factory/db_primitive_orm/headers/ -L/usr/local/Cellar/libpq/16.0/lib -lpthread -ldl -lpq";
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