#ifndef HTTPREQUESTMANAGER_H
#define HTTPREQUESTMANAGER_H
#include "HTTPGETRequest.h"
#include "HTTPPOSTRequest.h"

class HTTPRequestManager // A class factory for managing different request type objects
{
    private:
        std::map <string,HTTPRequest *,ignorecase> methods; // A map maping request method types to cloner objects
    public:
        HTTPRequestManager(); // Constructor
        // Returning a pointer to an HTTPRequest of the type correponding to p_method
        HTTPRequest * getService (TCPSocket * p_tcpSocket,string p_method); 
        ~HTTPRequestManager(); // Destructor
};


#endif // HTTPREQUESTMANAGER_H