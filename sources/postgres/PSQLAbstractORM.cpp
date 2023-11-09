#include <PSQLAbstractORM.h>
#include <PSQLController.h>

PSQLAbstractORM::PSQLAbstractORM (string _table_name,string _identifier_name)
{
    table_name = _table_name;
    identifier_name = _identifier_name;
    loaded = false;
    // psqlConnection = new PSQLConnection ("localhost",5432,"django_ostaz_15082023_old","postgres","postgres");
    // psqlQuery = NULL;
    // map<string, vector<string>> results  = psqlQuery->getResultAsString();
    // for (auto result: results) {
    //     std::cout << "ttt" << std::endl;
    //     std::cout << result.first  << std::endl;
    //     for (auto value: result.second) 
    //         std::cout << "\t\t" << value << std::endl;
    // }
}
string PSQLAbstractORM::getIdentifierName ()
{
    return identifier_name;
}

string PSQLAbstractORM::getTableName()
{
    return table_name;
}

bool PSQLAbstractORM::isLoaded ()
{
    return loaded;
}

void PSQLAbstractORM::lock_me()
{
    std::ostringstream ss;
    ss << std::this_thread::get_id() ;
    // printf ("lock_me: %p  -   %s \n",this,ss.str().c_str()); 
    lock.lock();
}
void PSQLAbstractORM::unlock_me()
{
    std::ostringstream ss;
    ss << std::this_thread::get_id() ;
    // printf ("unlock_me: %p  -   %s \n",this,ss.str().c_str()); 
    lock.try_lock();
    lock.unlock();
}
PSQLAbstractORM::~PSQLAbstractORM()
{
    // cout << "PSQLAbstractORM::~PSQLAbstractORM()" << endl;
    // if (psqlConnection != NULL) delete (psqlConnection);
    // if (psqlQuery != NULL) delete (psqlQuery);
    unlock_me();
    // std::ostringstream ss;
    // ss << std::this_thread::get_id() ;
    // printf ("deleting: %p  -   %s \n",this,ss.str().c_str()); 
}


PSQLQueryJoin::PSQLQueryJoin (string _data_source_name,vector <PSQLAbstractORM *> const & tables,vector <pair<pair<string,string>,pair<string,string>>> const & join_fields):
    PSQLQuery(NULL, "")
{
    string column_names = "";
    string table_names = "";
    string join_string = "";
    for ( int i =0 ; i  < tables.size() ; i ++)
    {
        if ( column_names != "") column_names += ",";
        column_names += tables[i]->getFromString();
        if ( table_names != "") table_names += ",";
        table_names += tables[i]->getTableName();
    }
    for ( int i = 0 ; i < join_fields.size() ; i ++)
    {
        cout << join_fields[i].first.first << endl;
        cout << join_fields[i].first.second << endl;
        cout << join_fields[i].second.first << endl;
        cout << join_fields[i].second.second << endl;
        if ( join_string != "") join_string += " and ";
        join_string += "\""+join_fields[i].first.first+"\"."+
        "\""+join_fields[i].first.second+"\" = "+
        "\""+join_fields[i].second.first+"\"."+
        "\""+join_fields[i].second.second+"\"";
    }

    string query = "select "+ column_names + " from " + table_names + " where " + join_string; 

    psqlConnection = psqlController.getPSQLConnection(_data_source_name);
    if (psqlConnection != NULL && psqlConnection->isAlive())
    {
        pgresult = PQexec(psqlConnection->getPGConnection(), query.c_str());

        if (PQresultStatus(pgresult) == 7 || PQresultStatus(pgresult) == 6)
        {

            cout << "Query ERROR: " << query << endl;
            cout << PQresultErrorMessage(pgresult) << endl
                 << "----------------------------\n";
        }

        if (PQresultStatus(pgresult) == PGRES_TUPLES_OK)
        {
            column_count = PQnfields(pgresult);
            result_count = PQntuples(pgresult);
        }
    }


}
vector <PSQLAbstractORM *> * PSQLQueryJoin::getORMs()
{
    return orms;

}
PSQLQueryJoin::~PSQLQueryJoin ()
{

}
