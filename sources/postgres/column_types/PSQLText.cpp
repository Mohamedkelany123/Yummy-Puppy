#include <PSQLText.h>

PSQLText::PSQLText():AbstractDatabaseColumn()
{
}
PSQLText::PSQLText(string _column_name):AbstractDatabaseColumn (_column_name)
{

}
string PSQLText::get_native_type(int index)
{
    if (index == 0 ) return "text";
    else if ( index == 1 ) return "varchar";
    else if ( index == 2 ) return "_varchar";
    else if ( index == 3 ) return "uuid";
    else if ( index == 4 ) return "date";
    else if ( index == 5 ) return "timestamptz";
    else if ( index == 6 ) return "int4range";
    else return "";
}

string PSQLText::genDeclaration ()
{
    return "\t\tstring "+field_name+";\n";
}
string PSQLText::genSetter (string class_name,int col_index)
{
    return  "\t\tvoid "+class_name+"::set_"+column_name+"( string _value) { update_flag.set("+std::to_string(col_index)+"); "+field_name+"=_value;} \n";
}
string PSQLText::genGetter (string class_name)
{
    return "\t\tstring "+class_name+"::get_"+column_name+"() { return "+field_name+";} \n";
}
string PSQLText::genSetterDef ()
{
    return  "\t\tvoid set_"+column_name+"( string _value); \n";
}
string PSQLText::genGetterDef ()
{
    return "\t\tstring get_"+column_name+"(); \n";
}
string PSQLText::genFieldConversion (string field)
{
    return field;
}

AbstractDatabaseColumn * PSQLText::clone (string _column_name)
{
    return new PSQLText (_column_name);
}

PSQLText::~PSQLText()
{

}
