#ifndef HTTPEXCEPTIONHANDLER_H
#define HTTPEXCEPTIONHANDLER_H

#include "HTTPResponse.h"

class HTTPExceptionHandler // Abstract class for handling HTTP Exceptions
{
    private:
    public:
        HTTPExceptionHandler(); // Constructor
        // Pure virtual method to handle exception and need to be implemented by all decendant classes
        virtual void handle (HTTPResponse * response) = 0;
        virtual ~HTTPExceptionHandler(); // Destructor
};

#endif