#include <AbstractDBConnection.h>

AbstractDBConnection::AbstractDBConnection(string p_host,
            unsigned short p_port,
            string p_database_name,
            string p_username,
            string p_password)
{
            host = p_host;
            port = p_port;
            database_name = p_database_name;
            username = p_username;
            password = p_password;
}

AbstractDBConnection::~AbstractDBConnection (){

}