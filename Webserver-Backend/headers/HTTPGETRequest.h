#ifndef HTTPGETREQUEST_H
#define HTTPGETREQUEST_H
#include "HTTPRequest.h"

// A class representing an HTTP GET Method Request Header
class HTTPGETRequest : public HTTPRequest // Inheriting from HTPRequest
{
    private:
    public:
        HTTPGETRequest(TCPSocket * p_tcpSocket); // Constructor
        virtual HTTPRequest * clone (TCPSocket * p_tcpSocket); // Clone and create new object 
        ~HTTPGETRequest(); // Destructor
};

#endif // HTTPGETREQUEST_H
