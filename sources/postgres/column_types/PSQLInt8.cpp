#include <PSQLInt8.h>

PSQLInt8::PSQLInt8():AbstractDatabaseColumn()
{

}
PSQLInt8::PSQLInt8(string column_name):AbstractDatabaseColumn (column_name)
{

}
string PSQLInt8::get_native_type()
{
    return "int8";
}

string PSQLInt8::genDeclaration ()
{
    return "\t\tlong "+column_name+";\n";
}
string PSQLInt8::genSetter (string class_name)
{
    return "\t\tvoid "+class_name+"::set_"+column_name+"( long _value) { "+column_name+"=_value;} \n";
}
string PSQLInt8::genGetter (string class_name)
{
    return "\t\tlong "+class_name+"::get_"+column_name+"() { return "+column_name+";} \n";
}
string PSQLInt8::genSetterDef ()
{
    return "\t\tvoid set_"+column_name+"( long _value); \n";
}
string PSQLInt8::genGetterDef ()
{
    return "\t\tlong get_"+column_name+"(); \n";
}
string PSQLInt8::genFieldConversion (string field)
{
    return "("+field+" == \"\" ? : stoi("+field+"))";
}

AbstractDatabaseColumn * PSQLInt8::clone (string column_name)
{
    return new PSQLInt8 (column_name);
}

PSQLInt8::~PSQLInt8()
{

}
