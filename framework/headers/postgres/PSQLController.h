#ifndef PSQLCONTROLLER_H
#define PSQLCONTROLLER_H
#include <PSQLConnectionManager.h>
#include <PSQLORMCache.h>
#include <TeamThread.h>


class PSQLController
{
    protected:
        PSQLConnectionManager * psqlConnectionManager;
        map <string, map<std::thread::id,PSQLORMCache *> *> * psqlORMCaches;
        map <string,pair<string,bool>> * insert_default_values;
        map <string,pair<string,bool>> * update_default_values;
        string checkDefaultDatasource(string data_source_name);
        bool checkInitialization();
        void createTeamThreadCache(string data_source_name);
        bool batch_mode;
        thread::id getTeamThreadId();

    public:
        void setBatchMode (bool _batch_mode=true);
        virtual void initialize(PSQLController * psqlControllerMaster)=0;
        PSQLController();
        bool addDataSource(string data_source_name,string _hostname,int _port,string _database,string _username,string _password);
        bool isDataSource(string data_source_name);
        PSQLConnection * getPSQLConnection(string data_source_name);
        bool releaseConnection (string data_source_name,PSQLConnection * psqlConnection);
        PSQLAbstractORM * addToORMCache(string name,PSQLAbstractORM * psqlAbstractORM, string data_source_name = "");
        PSQLAbstractORM * addToORMCache(PSQLAbstractORM * seeder, AbstractDBQuery * _psqlQuery, int _partition_number=-1, string data_source_name = "");
        void ORMCommitAll(bool parallel=false,bool transaction=false,bool clean_updates=false);
        void ORMCommit(bool parallel,bool transaction,bool clean_updates, string data_source_name = "");
        void ORMCommit_me(bool transaction=false,bool clean_updates=true);
        void ORMCommit_me(string data_source_name, bool transaction=false,bool clean_updates=true);
        void ORMFlush();
        void ORMFlush(string data_source_name);
        void ORMFlush_me();
        void ORMFlush_me(string data_source_name);
        int getDataSourceConnectionCount(string data_source_name);
        void setAllORMCacheThreads (int _threads_count);
        void setORMCacheThreads (int _threads_count, string data_source_name = "");
        void unlock_all_current_thread_orms();
        void unlock_current_thread_orms(string data_source_name = "");
        void addDefault(string name,string value, bool is_insert = true, bool is_func=false);
        map <string,pair<string,bool>> * getUpdateDefaultValues();
        map <string,pair<string,bool>> * getInsertDefaultValues();
        int getCacheCounter (string _data_source_name);
        PSQLConnectionManager * get_psqlConnectionManager();
        map <string, map<std::thread::id,PSQLORMCache *> *>  * get_psqlORMCaches();
        virtual ~PSQLController();
};

class PSQLControllerMaster: public PSQLController{
    private:
    public:
        PSQLControllerMaster();
        void initialize(PSQLController * psqlControllerMaster);
        ~PSQLControllerMaster();
};


class PSQLControllerSlave: public PSQLController{
    private:
    public:
        PSQLControllerSlave();
        void initialize(PSQLController * psqlControllerMaster);
        ~PSQLControllerSlave();
};


#ifdef SHARED_LIBRARY_FLAG
extern "C" PSQLControllerSlave psqlController; 
#else
extern "C" PSQLControllerMaster psqlController;
#endif

#endif