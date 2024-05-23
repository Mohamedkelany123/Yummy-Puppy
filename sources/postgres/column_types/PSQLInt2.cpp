#include <PSQLInt2.h>

PSQLInt2::PSQLInt2():AbstractDatabaseColumn()
{
}
PSQLInt2::PSQLInt2(string _column_name):AbstractDatabaseColumn (_column_name)
{

}
string PSQLInt2::get_native_type(int index)
{
    if ( index == 0 ) return "int2";
    else if (index == 1) return "_int2";
    else if (index == 2) return "int2range";
    else return "";
}
string PSQLInt2::genDeclaration ()
{
    return "\t\tshort "+field_name+";\n";
}
string PSQLInt2::genSetter (string class_name,int col_index)
{
    return "\t\tvoid "+class_name+"::set_"+column_name+"( short _value) { update_flag.set("+std::to_string(col_index)+"); "+field_name+"=_value;} \n";
}
string PSQLInt2::genGetter (string class_name)
{
    return "\t\tshort "+class_name+"::get_"+column_name+"() { return "+field_name+";} \n";
}
string PSQLInt2::genSetterDef ()
{
    return "\t\tvoid set_"+column_name+"( short _value); \n";
}
string PSQLInt2::genGetterDef ()
{
    return "\t\tshort get_"+column_name+"(); \n";
}
string PSQLInt2::genFieldConversion (string field,int col_index)
{
    return "("+field+" == \"\" ? (null_flag.set("+std::to_string(col_index)+").size() == 0 ) : stoi("+field+"))";
}
AbstractDatabaseColumn * PSQLInt2::clone (string _column_name)
{
    return new PSQLInt2 (_column_name);
}
PSQLInt2::~PSQLInt2()
{

}
