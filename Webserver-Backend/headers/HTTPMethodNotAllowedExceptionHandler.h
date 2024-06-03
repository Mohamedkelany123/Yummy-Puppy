#ifndef HTTPMETHODNOTALLOWEDEXCEPTIONHANDLER_H
#define HTTPMETHODNOTALLOWEDEXCEPTIONHANDLER_H

#include "HTTPExceptionHandler.h"
// Exception Handler for receiving an HTTP request with undefined method. 
//Basically sending back to the client status code 405
class HTTPMethodNotAllowedExceptionHandler: public HTTPExceptionHandler
{// Inheriting from the HTTPExceptionHandler Base class
    private:
    public:
        HTTPMethodNotAllowedExceptionHandler(); // Constructor
        //The Exception handler method        
        virtual void handle (TCPSocket * p_tcpSocket) ;
        ~HTTPMethodNotAllowedExceptionHandler(); // Destructor
};

#endif // HTTPMETHODNOTALLOWEDEXCEPTIONHANDLER_H