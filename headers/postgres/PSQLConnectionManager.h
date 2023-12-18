#ifndef PSQLCONNECTIONMANAGER_H
#define PSQLCONNECTIONMANAGER_H
#include <PSQLDataSource.h>

class PSQLConnectionManager 
{
    private:
        map <string,PSQLDataSource *> data_sources;  
        std::string default_data_source;
        std::mutex lock;      
    public:
        PSQLConnectionManager();
        bool addDataSource(string data_source_name,string _hostname,int _port,string _database,string _username,string _password);
        bool setDefaultDatasource(string _default_data_source);
        string getDefaultDatasource();
        PSQLConnection * getPSQLConnection(string data_source_name);
        bool releaseConnection (string data_source_name,PSQLConnection * psqlConnection);
        int getConnectionCount (string data_source_name);
        ~PSQLConnectionManager();
};


#endif