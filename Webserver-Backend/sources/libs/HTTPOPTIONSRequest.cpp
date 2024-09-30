#include "HTTPOPTIONSRequest.h"

// Constructor: Calling the base class constructor
HTTPOPTIONSRequest::HTTPOPTIONSRequest(TCPSocket * p_tcpSocket)
    : HTTPRequest(p_tcpSocket)
{
}

// A cloner method
HTTPRequest * HTTPOPTIONSRequest::clone(TCPSocket * p_tcpSocket)
{
    // Creates a new object of the same type and returns it to the caller
    return new HTTPOPTIONSRequest(p_tcpSocket);
}

HTTPOPTIONSRequest::~HTTPOPTIONSRequest() // Destructor
{
}
