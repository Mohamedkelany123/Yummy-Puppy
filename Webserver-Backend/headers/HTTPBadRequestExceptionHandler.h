#ifndef HTTPBADREQUESTEXCEPTIONHANLER_H
#define HTTPBADREQUESTEXCEPTIONHANLER_H

#include "HTTPExceptionHandler.h"

// Exception Handler for HTTP Request Not Acceptable. Basically sending back to the client status code 406
class HTTPBadRequestExceptionHandler: public HTTPExceptionHandler
{ // Inheriting from the HTTPExceptionHandler Base class
    private:
      int except_no;
      string error_messege;
    public:
        HTTPBadRequestExceptionHandler();
        HTTPBadRequestExceptionHandler(string error_messege_param); // Constructor
        //The Exception handler method
        virtual void handle (HTTPResponse* response) ;
        void setErrorMessage(string message);


        ~HTTPBadRequestExceptionHandler(); // Destructor
};

#endif // HTTPBADREQUESTEXCEPTIONHANLER_H