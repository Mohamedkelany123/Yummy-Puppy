#include <PSQLInt4.h>

PSQLInt4::PSQLInt4():AbstractDatabaseColumn()
{
}
PSQLInt4::PSQLInt4(string _column_name):AbstractDatabaseColumn (_column_name)
{

}
string PSQLInt4::get_native_type(int index)
{
    if ( index == 0 ) return "int4";
    else if (index == 1) return "_int4";
    else if (index == 2) return "int4range";
    else return "";
}

string PSQLInt4::genDeclaration ()
{
    return "\t\tint "+field_name+";\n";
}
string PSQLInt4::genSetter (string class_name,int col_index)
{
    return "\t\tvoid "+class_name+"::set_"+column_name+"( int _value) { update_flag.set("+std::to_string(col_index)+"); "+field_name+"=_value;} \n";
}
string PSQLInt4::genGetter (string class_name)
{
    return "\t\tint "+class_name+"::get_"+column_name+"() { return "+field_name+";} \n";
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

AbstractDatabaseColumn * PSQLInt4::clone (string _column_name)
{
    return new PSQLInt4 (_column_name);
}

PSQLInt4::~PSQLInt4()
{

}
