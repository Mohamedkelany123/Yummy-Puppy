#ifndef PSQLCONNECTION_H
#define PSQLCONNECTION_H

#include <PSQLQuery.h>
#include <AbstractDBConnection.h>
#include <common.h>
class PSQLConnection : public AbstractDBConnection
{
private:
    PGconn *psql_connection;
    vector<string> table_names;
    map <string,string> column_types;
    bool connect();
    void load_table_names();
    void load_column_types();


public:
    PSQLConnection(string p_host,
                   unsigned short p_port,
                   string p_database_name,
                   string p_username,
                   string p_password);
    bool isAlive();
    PGconn *getPGConnection();
    void *getConnection();
    string getTypeFromOID (string oid);
    AbstractDBQuery *executeQuery(string psql_query);
    bool executeUpdateQuery(string psql_query);
    vector<string> getTableNames();
    ~PSQLConnection();
};

#endif