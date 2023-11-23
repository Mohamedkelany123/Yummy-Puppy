#include <PSQLController.h>

PSQLController::PSQLController()
{
    psqlConnectionManager = new PSQLConnectionManager();
    psqlORMCache = new PSQLORMCache();
    printf ("PSQL Controller Initialized\n");
}
bool PSQLController::addDataSource(string data_source_name,string _hostname,int _port,string _database,string _username,string _password)
{
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
PSQLAbstractORM * PSQLController::addToORMCache(string name,PSQLAbstractORM * psqlAbstractORM)
{
    return psqlORMCache->add(name,psqlAbstractORM);
}
void PSQLController::ORMCommit(bool parallel,bool transaction,bool clean_updates)
{
    psqlORMCache->commit(parallel,transaction,clean_updates);
}
void PSQLController::ORMCommit(string name)
{
    psqlORMCache->commit(name);
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
    psqlORMCache->set_threads_count(_threads_count);
}

void PSQLController::unlock_current_thread_orms()
{
    psqlORMCache->unlock_current_thread_orms();
}

PSQLController::~PSQLController()
{
    delete (psqlORMCache);
    delete (psqlConnectionManager);
}


PSQLController psqlController;