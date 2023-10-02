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
    return "\t\tint "+field_name+";\n";
}
string PSQLNumeric::genSetter (string class_name)
{
    return "\t\tvoid "+class_name+"::set_"+column_name+"( int _value) { "+field_name+"=_value;} \n";
}
string PSQLNumeric::genGetter (string class_name)
{
    return "\t\tint "+class_name+"::get_"+column_name+"() { return "+field_name+";} \n";
}
string PSQLNumeric::genSetterDef ()
{
    return "\t\tvoid set_"+column_name+"( int _value); \n";
}
string PSQLNumeric::genGetterDef ()
{
    return "\t\tint get_"+column_name+"(); \n";
}
string PSQLNumeric::genFieldConversion (string field)
{
    return "("+field+" == \"\" ? : stod("+field+"))";
}

AbstractDatabaseColumn * PSQLNumeric::clone (string _column_name)
{
    return new PSQLNumeric (_column_name);
}

PSQLNumeric::~PSQLNumeric()
{

}
