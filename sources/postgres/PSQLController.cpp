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
string PSQLController::checkDefaultDatasource(string data_source_name){
    if (data_source_name == "") return psqlConnectionManager->getDefaultDatasource();
    else if (psqlORMCaches.find(data_source_name) == psqlORMCaches.end())
        throw std::runtime_error("ERROR :: Invalid Data Source Name");

    return data_source_name;
}
PSQLConnection * PSQLController::getPSQLConnection(string data_source_name)
{
    return psqlConnectionManager->getPSQLConnection(data_source_name);
}
bool PSQLController::releaseConnection (string data_source_name,PSQLConnection * psqlConnection)
{
    return psqlConnectionManager->releaseConnection(data_source_name,psqlConnection);
}
PSQLAbstractORM * PSQLController::addToORMCache(string name,PSQLAbstractORM * psqlAbstractORM, string data_source_name)
{
    data_source_name = checkDefaultDatasource(data_source_name);
    return psqlORMCaches[data_source_name]->add(name,psqlAbstractORM);
}
PSQLAbstractORM * PSQLController::addToORMCache(PSQLAbstractORM * seeder, AbstractDBQuery * _psqlQuery, int _partition_number, string data_source_name )
{
    data_source_name = checkDefaultDatasource(data_source_name);
    return psqlORMCaches[data_source_name]->add(seeder,_psqlQuery,_partition_number);
}

void PSQLController::ORMCommitAll(bool parallel,bool transaction,bool clean_updates)
{
    for (auto cache : psqlORMCaches){
        ORMCommit(parallel, transaction, clean_updates, cache.first);
    }
}
void PSQLController::ORMCommitAll(string name)
{
    for (auto cache : psqlORMCaches){
        ORMCommit(name, cache.first);
    }
}
void PSQLController::ORMCommit(bool parallel,bool transaction,bool clean_updates, string data_source_name)
{
    data_source_name = checkDefaultDatasource(data_source_name);
    psqlORMCaches[data_source_name]->commit(data_source_name, parallel,transaction,clean_updates);
}
void PSQLController::ORMCommit(string name, string data_source_name)
{
    data_source_name = checkDefaultDatasource(data_source_name);
    psqlORMCaches[data_source_name]->commit(data_source_name, name);
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
void PSQLController::setAllORMCacheThreads (int _threads_count)
{   
    for (auto cache : psqlORMCaches){
        setORMCacheThreads(_threads_count, cache.first);
    }
}

void PSQLController::setORMCacheThreads (int _threads_count, string data_source_name)
{
    data_source_name = checkDefaultDatasource(data_source_name);
    psqlORMCaches[data_source_name]->set_threads_count(_threads_count);
}

void PSQLController::unlock_all_current_thread_orms()
{
    for (auto cache : psqlORMCaches){
        unlock_current_thread_orms(cache.first);
    }
}

void PSQLController::unlock_current_thread_orms(string data_source_name)
{
    data_source_name = checkDefaultDatasource(data_source_name);
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
int PSQLController::getCacheCounter ()
{
    return psqlORMCaches["main"]->cache_counter;
}

void PSQLController::clear() {
    for (auto cache : psqlORMCaches){
        delete (cache.second);
    }
    delete (psqlConnectionManager);
    psqlConnectionManager = new PSQLConnectionManager();
} 

PSQLController psqlController;