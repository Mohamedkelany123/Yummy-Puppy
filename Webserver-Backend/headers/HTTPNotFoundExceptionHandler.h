#ifndef HTTPNOTFOUNDEXCEPTIONHANDLER_H
#define HTTPNOTFOUNDEXCEPTIONHANDLER_H

#include "HTTPExceptionHandler.h"
// Exception Handler for resource not found. Basically sending back to the client status code 404
class HTTPNotFoundExceptionHandler: public HTTPExceptionHandler
{// Inheriting from the HTTPExceptionHandler Base class
    private:
    public:
        HTTPNotFoundExceptionHandler(); // Constructor
        //The Exception handler method        
        virtual void handle (HTTPResponse* response) ;
        ~HTTPNotFoundExceptionHandler(); // Destructor
};

#endif // HTTPNOTFOUNDEXCEPTIONHANDLER_H