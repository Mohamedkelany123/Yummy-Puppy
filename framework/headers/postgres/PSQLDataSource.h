#ifndef PSQLDATASOURCE_H
#define PSQLDATASOURCE_H
#include <PSQLConnection.h>

// class PSQLDataSource 
// {
//     private:
//         string hostname;
//         int port;
//         string database;
//         string username;
//         string password;
//         map <uint64_t,PSQLConnection *> available_connections;
//         map <uint64_t,PSQLConnection *> reserved_connections;
//         std::mutex lock;
//     public:
//         PSQLDataSource(string _hostname,int _port,string _database,string _username,string _password);
//         PSQLConnection * getPSQLConnection();
//         bool releaseConnection (PSQLConnection * psqlConnection);
//         int getConnectionCount();
//         ~PSQLDataSource();
// };

#include <ResourceManager.h>


class PSQLDataSource: public ResourceManager <PSQLConnection>
{
    private:
        string hostname;
        int port;
        string database;
        string username;
        string password;
    public:
        PSQLDataSource(string _hostname,int _port,string _database,string _username,string _password);
        virtual ~PSQLDataSource();
};



#endif