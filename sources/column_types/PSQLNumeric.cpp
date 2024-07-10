#include <PSQLNumeric.h>

PSQLNumeric::PSQLNumeric():AbstractDatabaseColumn()
{
}
PSQLNumeric::PSQLNumeric(string _column_name):AbstractDatabaseColumn (_column_name)
{

}
string PSQLNumeric::get_native_type(int index)
{
    if ( index == 0 ) return "numeric";
    else if (index == 1) return "float8";
    else return "";
}

string PSQLNumeric::genDeclaration ()
{
    return "\t\tdouble "+field_name+";\n";
}
string PSQLNumeric::genSetter (string class_name,int col_index)
{
    return "\t\tvoid "+class_name+"::set_"+column_name+"( double _value, bool set_null) { if (seeder_readonly) return; update_flag.set("+std::to_string(col_index)+"); "+field_name+"=_value; if(set_null) null_flag.set("+std::to_string(col_index)+"); } \n";
}
string PSQLNumeric::genGetter (string class_name)
{
    return "\t\tdouble  "+class_name+"::get_"+column_name+"() { return "+field_name+";} \n";
}
string PSQLNumeric::genSetterDef ()
{
    return "\t\tvoid set_"+column_name+"( double _value, bool set_null = false); \n";
}
string PSQLNumeric::genGetterDef ()
{
    return "\t\tdouble get_"+column_name+"(); \n";
}
string PSQLNumeric::genFieldConversion (string field,int col_index)
{
    return "("+field+" == \"\" ? (null_flag.set("+std::to_string(col_index)+").size() == 0 )  : stod("+field+"))";
}

AbstractDatabaseColumn * PSQLNumeric::clone (string _column_name)
{
    return new PSQLNumeric (_column_name);
}

PSQLNumeric::~PSQLNumeric()
{

}
