#include <PSQLAbstractQueryIterator.h>
#include <PSQLController.h>

PSQLAbstractQueryIterator::PSQLAbstractQueryIterator(string _data_source_name,string _table_name)
{
    data_source_name = _data_source_name;
    table_name = _table_name;
    psqlConnection = psqlController.getPSQLConnection(data_source_name);
    sql = "select * from "+_table_name;
    cout << sql << endl;
    conditions = "";
}
void PSQLAbstractQueryIterator::setNativeSQL(string _sql)
{
    sql=_sql;
}
void PSQLAbstractQueryIterator::filter (const Expression & e)
{
    conditions = " where " + e.generate();

}
bool PSQLAbstractQueryIterator::execute()
{
    psqlQuery = psqlConnection->executeQuery(sql+conditions);
    if (psqlQuery != NULL) return true;
    else return false;
}
PSQLAbstractQueryIterator::~PSQLAbstractQueryIterator()
{
    psqlController.releaseConnection(data_source_name,psqlConnection);
    if ( psqlQuery != NULL) delete (psqlQuery);
    
}
