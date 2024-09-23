#ifndef PSQLCONTROLLER_H
#define PSQLCONTROLLER_H
#include <PSQLConnectionManager.h>
#include <PSQLORMCache.h>


class PSQLController
{
    protected:
        PSQLConnectionManager * psqlConnectionManager;
        map <string, PSQLORMCache *> * psqlORMCaches;
        map <string,pair<string,bool>> * insert_default_values;
        map <string,pair<string,bool>> * update_default_values;
        string checkDefaultDatasource(string data_source_name);
        bool checkInitialization ();
    public:
        void initializeFromMater(PSQLController * psqlControllerMaster)=0;
        PSQLController();
        bool addDataSource(string data_source_name,string _hostname,int _port,string _database,string _username,string _password);
        PSQLConnection * getPSQLConnection(string data_source_name);
        bool releaseConnection (string data_source_name,PSQLConnection * psqlConnection);
        PSQLAbstractORM * addToORMCache(string name,PSQLAbstractORM * psqlAbstractORM, string data_source_name = "");
        PSQLAbstractORM * addToORMCache(PSQLAbstractORM * seeder, AbstractDBQuery * _psqlQuery, int _partition_number=-1, string data_source_name = "");
        void ORMCommitAll(bool parallel=false,bool transaction=false,bool clean_updates=false);
        void ORMCommitAll(string name);
        void ORMCommit(string name,long id);
        void ORMCommit(bool parallel,bool transaction,bool clean_updates, string data_source_name = "");
        void ORMCommit(string name, string data_source_name = "");
        void ORMFlush();
        void ORMFlush(string name);
        void ORMFlush(string name,long id);
        int getDataSourceConnectionCount(string data_source_name);
        void setAllORMCacheThreads (int _threads_count);
        void setORMCacheThreads (int _threads_count, string data_source_name = "");
        void unlock_all_current_thread_orms();
        void unlock_current_thread_orms(string data_source_name = "");
        void addDefault(string name,string value, bool is_insert = true, bool is_func=false);
        map <string,pair<string,bool>> getUpdateDefaultValues();
        map <string,pair<string,bool>> getInsertDefaultValues();
        int getCacheCounter (string _data_source_name);
        void clear();
        virtual ~PSQLController();
};

class PSQLControllerMaster: public PSQLController{
    private:
    public:
        PSQLControllerMaster();
        void initializeFromMater(PSQLController * psqlControllerMaster);
        ~PSQLControllerMaster();
};


class PSQLControllerSlave: public PSQLController{
    private:
    public:
        PSQLControllerSlave();
        void initializeFromMaster(PSQLController * psqlControllerMaster);
        ~PSQLControllerSlave();
};

#ifdef SHARED_LIBRARY_FLAG
extern "C" PSQLControllerSlave psqlController; 
#else
extern "C" PSQLControllerMaster psqlController;
#endif
// PSQLController psqlController;

#endif