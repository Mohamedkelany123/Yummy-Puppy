#include <PSQLInt4.h>

PSQLInt4::PSQLInt4():AbstractDatabaseColumn()
{
}
PSQLInt4::PSQLInt4(string column_name):AbstractDatabaseColumn (column_name)
{

}
string PSQLInt4::get_native_type()
{
    return "int4";
}

string PSQLInt4::genDeclaration ()
{
    return "\t\tint "+column_name+";\n";
}
string PSQLInt4::genSetter (string class_name)
{
    return "\t\tvoid "+class_name+"::set_"+column_name+"( int _value) { "+column_name+"=_value;} \n";
}
string PSQLInt4::genGetter (string class_name)
{
    return "\t\tint "+class_name+"::get_"+column_name+"() { return "+column_name+";} \n";
}
string PSQLInt4::genSetterDef ()
{
    return "\t\tvoid set_"+column_name+"( int _value); \n";
}
string PSQLInt4::genGetterDef ()
{
    return "\t\tint get_"+column_name+"(); \n";
}
string PSQLInt4::genFieldConversion (string field)
{
    return "("+field+" == \"\" ? : stoi("+field+"))";
}

AbstractDatabaseColumn * PSQLInt4::clone (string column_name)
{
    return new PSQLInt4 (column_name);
}

PSQLInt4::~PSQLInt4()
{

}
