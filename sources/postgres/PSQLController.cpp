#include <PSQLController.h>

PSQLController::PSQLController()
{
    psqlConnectionManager = new PSQLConnectionManager();
    printf ("PSQL Controller Initialized\n");
}
bool PSQLController::addDataSource(string data_source_name,string _hostname,int _port,string _database,string _username,string _password)
{
    psqlORMCaches[data_source_name] = new PSQLORMCache();
    return psqlConnectionManager->addDataSource(data_source_name,_hostname,_port,_database,_username,_password);
}
PSQLConnection * PSQLController::getPSQLConnection(string data_source_name)
{
    return psqlConnectionManager->getPSQLConnection(data_source_name);
}
bool PSQLController::releaseConnection (string data_source_name,PSQLConnection * psqlConnection)
{
    return psqlConnectionManager->releaseConnection(data_source_name,psqlConnection);
}
PSQLAbstractORM * PSQLController::addToORMCache(string data_source_name, string name,PSQLAbstractORM * psqlAbstractORM)
{
    return psqlORMCaches[data_source_name]->add(name,psqlAbstractORM);
}
void PSQLController::ORMCommit(bool parallel,bool transaction,bool clean_updates)
{
    for (auto cache : psqlORMCaches){
        ORMCommit(cache.first, parallel, transaction, clean_updates);
    }
}
void PSQLController::ORMCommit(string name)
{
    for (auto cache : psqlORMCaches){
        ORMCommit(cache.first,name);
    }
}
void PSQLController::ORMCommit(string data_source_name, bool parallel,bool transaction,bool clean_updates)
{
    psqlORMCaches[data_source_name]->commit(parallel,transaction,clean_updates);
}
void PSQLController::ORMCommit(string data_source_name, string name)
{
    psqlORMCaches[data_source_name]->commit(name);
}
void PSQLController::ORMCommit(string name,long id)
{

}
void PSQLController::ORMFlush()
{

}
void PSQLController::ORMFlush(string name)
{

}
void PSQLController::ORMFlush(string name,long id)
{

}

int PSQLController::getDataSourceConnectionCount(string data_source_name)
{
    return psqlConnectionManager->getConnectionCount(data_source_name);

}
void PSQLController::setORMCacheThreads (int _threads_count)
{   
    for (auto cache : psqlORMCaches){
        setORMCacheThreads(cache.first, _threads_count);
    }
}

void PSQLController::setORMCacheThreads (string data_source_name, int _threads_count)
{
    psqlORMCaches[data_source_name]->set_threads_count(_threads_count);
}

void PSQLController::unlock_current_thread_orms()
{
    for (auto cache : psqlORMCaches){
        unlock_current_thread_orms(cache.first);
    }
}

void PSQLController::unlock_current_thread_orms(string data_source_name)
{
    psqlORMCaches[data_source_name]->unlock_current_thread_orms();
}

void PSQLController::addDefault(string name,string value, bool is_insert, bool is_func)
{
    pair<string, bool> p; 
    p.first = value;
    p.second = is_func;
    if ( is_insert )
        insert_default_values[name]= p;
    else update_default_values[name]= p;
}

map <string,pair<string,bool>> PSQLController::getUpdateDefaultValues()
{
    return update_default_values;
}
map <string,pair<string,bool>> PSQLController::getInsertDefaultValues()
{
    return insert_default_values;

}

PSQLController::~PSQLController()
{
    for (auto cache : psqlORMCaches){
        delete (cache.second);
    }
    delete (psqlConnectionManager);
}


PSQLController psqlController;