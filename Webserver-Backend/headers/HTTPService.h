#ifndef HTTPSERVICE_H
#define HTTPSERVICE_H
#include "includes.h"
#include "TCPSocket.h"
#include "HTTPRequest.h"

class HTTPService // An abstract class representing an HTTP Service
{
    public:
        // Constructor setting up the objec data members
        HTTPService();
        // A pure virtual method of executing the HTTP back end which should generate the HTTP response body and send it to the client over the current connection TCP socket
        virtual bool execute(HTTPRequest * p_httpRequest,TCPSocket * p_tcpSocket) = 0;
        // A pure virtual method that should be implemented by all descendants to clone and create new object        
        virtual HTTPService * clone () = 0;
        virtual ~HTTPService(); // Destructor
};

#endif // HTTPSERVICE_H
