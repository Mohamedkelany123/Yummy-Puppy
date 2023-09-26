#ifndef ABSTRACTDBCONNECTION_H
#define ABSTRACTDBCONNECTION_H

#include <AbstractDBQuery.h>

class AbstractDBConnection
{
protected:
    string host;
    unsigned short port;
    string database_name;
    string username;
    string password;
    virtual bool connect() = 0;

public:
    AbstractDBConnection(string p_host,
                         unsigned short p_port,
                         string p_database_name,
                         string p_username,
                         string p_password);
    virtual bool isAlive() = 0;
    string get_name() { return database_name; }
    virtual void *getConnection() = 0;
    virtual AbstractDBQuery *executeQuery(string psql_query) = 0;
    //virtual void operator>>(vector<AbstractDBConnection *> abstractDBConnection) = 0;
    virtual ~AbstractDBConnection();
};

#endif