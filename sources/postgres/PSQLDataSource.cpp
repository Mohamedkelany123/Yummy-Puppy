#include <PSQLDataSource.h>

PSQLDataSource::PSQLDataSource(string _hostname,int _port,string _database,string _username,string _password)
{
    hostname =_hostname;
    port = _port;
    database = _database;
    username = _username;
    password = _password;
}
PSQLConnection * PSQLDataSource::getPSQLConnection()
{
    std::lock_guard<std::mutex> guard(lock);
    PSQLConnection * psqlConnection  =  NULL;
    if (available_connections.empty())
    {
        psqlConnection  =  new PSQLConnection (hostname,port,database,username,password);
        reserved_connections[(uint64_t)psqlConnection] = psqlConnection;
    }
    else
    {
        map<uint64_t,PSQLConnection *>::iterator  victim = available_connections.begin();
        available_connections.erase(victim->first);
        reserved_connections[victim->first] = victim->second;
        psqlConnection = victim->second;
    }
    return psqlConnection;
}
bool PSQLDataSource::releaseConnection (PSQLConnection * psqlConnection)
{
    std::lock_guard<std::mutex> guard(lock);
    if (reserved_connections.find((uint64_t)psqlConnection) != reserved_connections.end())
    {
        available_connections[(uint64_t)psqlConnection] = psqlConnection;
        reserved_connections.erase((uint64_t)psqlConnection);
        return true;
    }
    return false;
}
PSQLDataSource::~PSQLDataSource()
{
    std::lock_guard<std::mutex> guard(lock);
    for (auto available_connection: available_connections) 
        delete (available_connection.second);

    for (auto reserved_connection: reserved_connections) 
        delete (reserved_connection.second);

}
