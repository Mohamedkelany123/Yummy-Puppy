#include <PSQLInt8.h>

PSQLInt8::PSQLInt8():AbstractDatabaseColumn()
{

}
PSQLInt8::PSQLInt8(string _column_name):AbstractDatabaseColumn (_column_name)
{

}
string PSQLInt8::get_native_type(int index)
{
    if (index == 0 ) return "int8";
    else return "";
}

string PSQLInt8::genDeclaration ()
{
    return "\t\tlong "+field_name+";\n";
}
string PSQLInt8::genSetter (string class_name,int col_index)
{
    return "\t\tvoid "+class_name+"::set_"+column_name+"( long _value) { update_flag.set("+std::to_string(col_index)+"); "+field_name+"=_value;} \n";
}
string PSQLInt8::genGetter (string class_name)
{
    return "\t\tlong "+class_name+"::get_"+column_name+"() { return "+field_name+";} \n";
}
string PSQLInt8::genSetterDef ()
{
    return "\t\tvoid set_"+column_name+"( long _value); \n";
}
string PSQLInt8::genGetterDef ()
{
    return "\t\tlong get_"+column_name+"(); \n";
}
string PSQLInt8::genFieldConversion (string field,int col_index)
{
    return "("+field+" == \"\" ? (null_flag.set("+std::to_string(col_index)+").size() == 0 ): stoi("+field+"))";
}

AbstractDatabaseColumn * PSQLInt8::clone (string _column_name)
{
    return new PSQLInt8 (_column_name);
}

PSQLInt8::~PSQLInt8()
{

}
