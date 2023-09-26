#ifndef PSQLCONNECTIONMANAGER_H
#define PSQLCONNECTIONMANAGER_H
#include <PSQLDataSource.h>

class PSQLConnectionManager 
{
    private:
        map <string,PSQLDataSource *> data_sources;        
    public:
        PSQLConnectionManager();
        bool addDataSource(string data_source_name,string _hostname,int _port,string _database,string _username,string _password);
        PSQLConnection * getPSQLConnection(string data_source_name);
        bool releaseConnection (string data_source_name,PSQLConnection * psqlConnection);
        ~PSQLConnectionManager();
};


#endif