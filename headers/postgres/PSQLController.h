#ifndef PSQLCONTROLLER_H
#define PSQLCONTROLLER_H
#include <PSQLConnectionManager.h>
#include <PSQLORMCache.h>

class PSQLController
{
    private:
        PSQLConnectionManager * psqlConnectionManager;
        PSQLORMCache * psqlORMCache;
    public:
        PSQLController();
        bool addDataSource(string data_source_name,string _hostname,int _port,string _database,string _username,string _password);
        PSQLConnection * getPSQLConnection(string data_source_name);
        bool releaseConnection (string data_source_name,PSQLConnection * psqlConnection);
        void addToORMCache(string name,PSQLAbstractORM * psqlAbstractORM);
        void ORMCommit();
        void ORMCommit(string name);
        void ORMCommit(string name,long id);
        void ORMFlush();
        void ORMFlush(string name);
        void ORMFlush(string name,long id);
        int getDataSourceConnectionCount(string data_source_name);
        ~PSQLController();
};

extern "C" PSQLController psqlController;
// PSQLController psqlController;

#endif