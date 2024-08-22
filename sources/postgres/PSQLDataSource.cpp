#include <PSQLDataSource.h>

PSQLDataSource::PSQLDataSource(string _hostname,int _port,string _database,string _username,string _password):ResourceManager <PSQLConnection>([](ResourceManager <PSQLConnection> * me)->PSQLConnection* {
    PSQLDataSource * mee = (PSQLDataSource *)me;
    cout << mee->hostname << endl;
    cout << mee->port << endl;
    cout << mee->database << endl;
    cout << mee->username << endl;
    cout << mee->password << endl;
    return new PSQLConnection (mee->hostname,mee->port,mee->database,mee->username,mee->password);
})
{
    hostname =_hostname;
    port = _port;
    database = _database;
    username = _username;
    password = _password;
}
PSQLDataSource::~PSQLDataSource(){}


// PSQLDataSource::PSQLDataSource(string _hostname,int _port,string _database,string _username,string _password)
// {
//     hostname =_hostname;
//     port = _port;
//     database = _database;
//     username = _username;
//     password = _password;
// }
// PSQLConnection * PSQLDataSource::getPSQLConnection()
// {
//     std::lock_guard<std::mutex> guard(lock);
//     PSQLConnection * psqlConnection  =  NULL;
//     if (available_connections.empty())
//     {
//         psqlConnection  =  new PSQLConnection (hostname,port,database,username,password);
//         reserved_connections[(uint64_t)psqlConnection] = psqlConnection;
//     }
//     else
//     {
//         map<uint64_t,PSQLConnection *>::iterator  victim = available_connections.begin();
//         reserved_connections[victim->first] = victim->second;
//         psqlConnection = victim->second;
//         available_connections.erase(victim->first); // erase will fuck up the victim on mac, logically it should not :)
//     }
//     return psqlConnection;
// }
// bool PSQLDataSource::releaseConnection (PSQLConnection * psqlConnection)
// {
//     std::lock_guard<std::mutex> guard(lock);
//     if (reserved_connections.find((uint64_t)psqlConnection) != reserved_connections.end())
//     {
//         available_connections[(uint64_t)psqlConnection] = psqlConnection;
//         reserved_connections.erase((uint64_t)psqlConnection);
//         return true;
//     }
//     return false;
// }
// int PSQLDataSource::getConnectionCount()
// {
//     return available_connections.size()+reserved_connections.size();
// }
// PSQLDataSource::~PSQLDataSource()
// {
//     std::lock_guard<std::mutex> guard(lock);
//     for (auto available_connection: available_connections) 
//         delete (available_connection.second);

//     for (auto reserved_connection: reserved_connections) 
//         delete (reserved_connection.second);

// }
