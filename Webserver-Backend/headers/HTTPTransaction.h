#ifndef HTTPTRANSACTION_H
#define HTTPTRANSACTION_H
#include "HTTPServiceManager.h"
#include "HTTPRequest.h"
#include "HTTPRequestManager.h"
#include <MiddlewareManager.h>

// A class encapsulating all the details of an HTTP Transaction. It inherits from Thread as every HTTP transaction should be executed as a detached paralled independant thread
class HTTPTransaction
{
    private:
        TCPSocket * tcpSocket; // TCP socket of the target HTTP transaction
        HTTPServiceManager * httpServiceManager; // Pointer to the HTTPServiceManager to be able to fetch services
        HTTPRequestManager * httpRequestManager; // Pointer to the HTTPRequestManager to be able to fetch a request handler based on the request method type
        thread * th;
        MiddlewareManager * middlewareManager;
        HTTPRequest * fetchHTTPRequest (); // Return an HTTPRequest object whose type depends on the HTTPHeader method
        void process(); // Process the HTTP transaction
    public:
        // Constructor setting up the data members of the HTTPTRansaction object
        HTTPTransaction (TCPSocket * p_tcpSocket,HTTPServiceManager * p_httpServiceManager,HTTPRequestManager * p_httpRequestManager,MiddlewareManager * _middlewareManager);
        void  threadMainBody (); // The main thread body that executed all the logic for the detached thread
        static void startHTTPTransaction(HTTPTransaction * me);
        void setThread(thread * p_th);
        bool isRunning();
        void waitForRunToFinish();
        ~HTTPTransaction(); // Destructor
};

#endif //HTTPTRANSACTION_H
