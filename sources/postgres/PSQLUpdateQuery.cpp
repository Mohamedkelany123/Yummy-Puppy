#include <PSQLUpdateQuery.h>


PSQLUpdateQuery::PSQLUpdateQuery(string _data_source_name,string _table_name,Expression const & e,map <string,string> set_map)
{
    table_name = _table_name;
    conditions = "";
    set_string = "";
    data_source_name = _data_source_name;
    filter(e);
    set(set_map);
}
void PSQLUpdateQuery::filter ( Expression const & e)
{
    if ( conditions == "")
        conditions = " where " + e.generate();
    else conditions += " and "+ e.generate();

}
void PSQLUpdateQuery::set (map <string,string> set_map)
{
    for (auto const& x : set_map)
    {
        if ( set_string != "") set_string = ",";
        set_string += x.first+"='"+x.second+"'";
    }
}
bool PSQLUpdateQuery::update (PSQLConnection * _psqlConnection)
{
    string sql_update = "update "+table_name+" set "+set_string+conditions;
    PSQLConnection * psqlConnection = _psqlConnection;
    if (_psqlConnection == NULL) psqlConnection = psqlController.getPSQLConnection(data_source_name);
    bool return_flag=psqlConnection->executeUpdateQuery(sql_update);
    if ( _psqlConnection == NULL) psqlController.releaseConnection(data_source_name,psqlConnection);
    return return_flag;
}
PSQLUpdateQuery::~PSQLUpdateQuery()
{

}