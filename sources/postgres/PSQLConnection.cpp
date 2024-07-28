#include <PSQLConnection.h>


void PSQLConnection::load_table_names()
{
    AbstractDBQuery *psqlQuery = this->executeQuery("SELECT tablename FROM pg_catalog.pg_tables WHERE schemaname != 'pg_catalog' AND schemaname != 'information_schema' and schemaname != 'nonreplicated' and schemaname = 'public'");
    for (;psqlQuery->fetchNextRow();)
    {
        for (int c = 0; c < psqlQuery->getColumnCount(); c++)
        {
            this->table_names.push_back(psqlQuery->getResultField(c));
        }
    } 
    delete (psqlQuery);
}

void PSQLConnection::load_column_types()
{
    AbstractDBQuery *psqlQuery = this->executeQuery("SELECT oid,typname from pg_type");
    for (;psqlQuery->fetchNextRow();)
    {
        this->column_types[psqlQuery->getResultField(0)] = psqlQuery->getResultField(1);
    }
    delete (psqlQuery);
}


bool PSQLConnection::connect()
{
    if ( psql_connection != NULL )
    {
        PQfinish(psql_connection);
        psql_connection = NULL;
    }
    string connection_string = "postgresql://" + username + ":" + password + "@" + host + ":" + to_string(port) + "/" + database_name;
    psql_connection = PQconnectdb(connection_string.c_str());
    return (psql_connection != NULL);
}

PSQLConnection::PSQLConnection(string p_host,
                               unsigned short p_port,
                               string p_database_name,
                               string p_username,
                               string p_password) : AbstractDBConnection(p_host, p_port, p_database_name, p_username, p_password)
{
    // host = p_host;
    // port = p_port;
    // database_name = p_database_name;
    // username = p_username;
    // password = p_password;
    psql_connection = NULL;
    this->connect();
    if (isAlive()) 
    {
        load_table_names();
        load_column_types();
    }
}
bool PSQLConnection::isAlive()
{
    if (PQstatus(psql_connection) == CONNECTION_OK) return true;
    else
    {
        cout << "Reconnecting ................"<< endl;
        if (!this->connect() || PQstatus(psql_connection) != CONNECTION_OK)
            cout <<"Reconnecting failed "<< endl;
        return (PQstatus(psql_connection) == CONNECTION_OK);
    }
}

PGconn *PSQLConnection::getPGConnection()
{
    return (PGconn *)this->getConnection();
}

void *PSQLConnection::getConnection()
{
    return psql_connection;
}

string PSQLConnection::getTypeFromOID (string oid)
{
    return column_types[oid];
}
AbstractDBQuery *PSQLConnection::executeQuery(string psql_query)
{
    return new PSQLQuery(this, psql_query);
}

bool PSQLConnection::executeUpdateQuery(string psql_query)
{
    // cout << "________________________" << psql_query << endl;
    PGresult * pgresult = PQexec(this->getPGConnection(), psql_query.c_str());
    // cout << "pgresults status: "<< PQresultStatus(pgresult) << endl;
    if (PQresultStatus(pgresult) == 7 || PQresultStatus(pgresult) == 6)
    {

        cout << "Query ERROR: " << psql_query << endl;
        cout << PQresultErrorMessage(pgresult) << endl
                << "----------------------------\n";
        return false;
    }
    else return true;
}

long PSQLConnection::executeInsertQuery(string psql_query)
{
    // cout << "________________________" << psql_query << endl;
    PGresult * pgresult = PQexec(this->getPGConnection(), psql_query.c_str());
    if (PQresultStatus(pgresult) == 7 || PQresultStatus(pgresult) == 6)
    {

        cout << "Query ERROR: " << psql_query << endl;
        cout << PQresultErrorMessage(pgresult) << endl
                << "----------------------------\n";
        return -1;
    }
    else 
    {
        // if (PQresultStatus(pgresult) == PGRES_TUPLES_OK)
        // {
        //     int column_count = PQnfields(pgresult);
        //     int result_count = PQntuples(pgresult);
        // }
        string s = PQgetvalue(pgresult, 0, 0);
        // cout << s << endl;
        // printf ("%lu\n",atol(s.c_str()));
        return atol(s.c_str());
    }
}

vector<string> PSQLConnection::getTableNames()
{
    return (this->table_names);
}


void PSQLConnection::startTransaction ()
{
    PQexec(psql_connection,"SET AUTOCOMMIT OFF");
    PQexec(psql_connection,"BEGIN");
    PQexec(psql_connection, "SET CONSTRAINTS ALL IMMEDIATE");
}
void PSQLConnection::commitTransaction()
{
    PQexec(psql_connection,"COMMIT");
}
void PSQLConnection::rollbackTransaction()
{
    PQexec(psql_connection,"ROLLBACK");
}

PSQLConnection::~PSQLConnection()
{
    if (psql_connection != NULL)
        PQfinish(psql_connection);
}
