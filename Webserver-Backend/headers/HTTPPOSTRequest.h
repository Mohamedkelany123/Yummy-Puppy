#ifndef HTTPPOSTREQUEST_H
#define HTTPPOSTREQUEST_H
#include "HTTPRequest.h"

// A class representing an HTTP POST Method Request Header
class HTTPPOSTRequest : public HTTPRequest
{
    private:
//        void readAndParseBody(); // Parse and add a header item to the header map
    public:
        HTTPPOSTRequest(TCPSocket * p_tcpSocket); // Constructor
        virtual void readAndParse(string initial_header, long sz); // Read header from socket and parse it
        virtual HTTPRequest * clone (TCPSocket * p_tcpSocket); // Clone and create new object 
        ~HTTPPOSTRequest(); // Destructor
};

#endif // HTTPPOSTREQUEST_H
