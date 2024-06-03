#include "HTTPGETRequest.h"
#include "TCPSocket.h" 

// Constructor:  calling the base class constructor
HTTPGETRequest::HTTPGETRequest(TCPSocket * p_tcpSocket): HTTPRequest(p_tcpSocket)
{
}
// A cloner method
HTTPRequest * HTTPGETRequest::clone (TCPSocket * p_tcpSocket)
{
    // Creates a new object of the same type and return it to the caller
    return new HTTPGETRequest(p_tcpSocket);
}
HTTPGETRequest::~HTTPGETRequest() // Destructor
{
}
