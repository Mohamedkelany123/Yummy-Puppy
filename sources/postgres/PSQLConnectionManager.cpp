#include <PSQLConnectionManager.h>


PSQLConnectionManager::PSQLConnectionManager()
{

}
bool PSQLConnectionManager::addDataSource(string data_source_name,string _hostname,int _port,string _database,string _username,string _password)
{
    if (data_sources.find(data_source_name) != data_sources.end()) return false;
    else 
    {
        data_sources[data_source_name] = new PSQLDataSource(_hostname,_port,_database,_username,_password);
        return true;
    }
}
PSQLConnection * PSQLConnectionManager::getPSQLConnection(string data_source_name)
{
    if (data_sources.find(data_source_name) != data_sources.end()) 
    {
        return data_sources[data_source_name]->getPSQLConnection();
    }
    else return NULL;
}
bool PSQLConnectionManager::releaseConnection (string data_source_name,PSQLConnection * psqlConnection)
{
    if (data_sources.find(data_source_name) != data_sources.end())
    {
        data_sources[data_source_name]->releaseConnection(psqlConnection);
        return true;
    }
    else return false;
}
PSQLConnectionManager::~PSQLConnectionManager()
{
    for (auto data_source: data_sources) 
    delete (data_source.second);
 
}
