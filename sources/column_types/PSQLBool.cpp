#include <PSQLBool.h>

PSQLBool::PSQLBool():AbstractDatabaseColumn()
{
}
PSQLBool::PSQLBool(string _column_name):AbstractDatabaseColumn (_column_name)
{

}
string PSQLBool::get_native_type(int index)
{
    if (index == 0 ) return "bool";
    else return "";
}


string PSQLBool::genDeclaration ()
{
    return "\t\tbool "+field_name+";\n";
}
string PSQLBool::genSetter (string class_name,int col_index)
{
    return  "\t\tvoid "+class_name+"::set_"+column_name+"( bool _value, bool set_null) { update_flag.set("+std::to_string(col_index)+"); "+field_name+"=_value; if(set_null) null_flag.set("+std::to_string(col_index)+"); } \n"+
            "\t\tvoid "+class_name+"::set_"+column_name+"( string _value, bool set_null) { update_flag.set("+std::to_string(col_index)+"); if (_value == \"f\") "+field_name+"=true; else if (_value == \"t\") "+field_name+"= false; if(set_null) null_flag.set("+std::to_string(col_index)+"); } \n";
}
string PSQLBool::genGetter (string class_name)
{
    return "\t\tbool "+class_name+"::get_"+column_name+"() { return "+field_name+";} \n";
}
string PSQLBool::genSetterDef ()
{
    return  "\t\tvoid set_"+column_name+"( bool _value, bool set_null = false); \n"+
            "\t\tvoid set_"+column_name+"( string _value, bool set_null = false); \n";
}
string PSQLBool::genGetterDef ()
{
    return "\t\tbool get_"+column_name+"(); \n";
}
string PSQLBool::genFieldConversion (string field,int col_index)
{
    return "(("+field+" == \"f\") ? false: true)";
}

AbstractDatabaseColumn * PSQLBool::clone (string _column_name)
{
    return new PSQLBool (_column_name);
}

PSQLBool::~PSQLBool()
{

}
