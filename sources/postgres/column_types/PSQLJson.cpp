#include <PSQLJson.h>

PSQLJson::PSQLJson():AbstractDatabaseColumn()
{
}
PSQLJson::PSQLJson(string _column_name):AbstractDatabaseColumn (_column_name)
{

}
string PSQLJson::get_native_type(int index)
{
    if (index == 0 ) return "jsonb";
    else return "";
}

string PSQLJson::genDeclaration ()
{
    return "\t\tjson "+field_name+";\n";
}
string PSQLJson::genSetter (string class_name,int col_index)
{
    return  "\t\tvoid "+class_name+"::set_"+column_name+"( json _value) { update_flag.set("+std::to_string(col_index)+"); "+field_name+"=_value;} \n"+
        "\t\tvoid "+class_name+"::set_"+column_name+"( string _value) { update_flag.set("+std::to_string(col_index)+"); "+field_name+"=json::parse(_value);} \n";
}
string PSQLJson::genGetter (string class_name)
{
    return "\t\tjson "+class_name+"::get_"+column_name+"() { return "+field_name+";} \n";
}
string PSQLJson::genSetterDef ()
{
    return  "\t\tvoid set_"+column_name+"( json _value); \n"+
    "\t\tvoid set_"+column_name+"( string _value); \n";
}
string PSQLJson::genGetterDef ()
{
    return "\t\tjson get_"+column_name+"(); \n";
}
string PSQLJson::genFieldConversion (string field,int col_index)
{
    return "json::parse("+field+")";
}

AbstractDatabaseColumn * PSQLJson::clone (string _column_name)
{
    return new PSQLJson (_column_name);
}

PSQLJson::~PSQLJson()
{

}
