#include "HTTPTransaction.h"
#include "HTTPGETRequest.h"
#include "HTTPNotAcceptableExceptionHandler.h"
#include "HTTPMethodNotAllowedExceptionHandler.h"

// Constructor: initailizing data members and calling the Thread Base class constructor
HTTPTransaction::HTTPTransaction(TCPSocket *p_tcpSocket, HTTPServiceManager *p_httpServiceManager, HTTPRequestManager *p_httpRequestManager, MiddlewareManager *_middlewareManager)
{
    httpServiceManager = p_httpServiceManager;
    tcpSocket = p_tcpSocket;
    httpRequestManager = p_httpRequestManager;
    middlewareManager = _middlewareManager;
};
// Instantiate the HTTPRequest object based on the method type of the request
HTTPRequest *HTTPTransaction::fetchHTTPRequest()
{
    // buffer to read very small amount of data from socket. Enough to get the method name
    char buffer[1024];
    memset(buffer, 0, 1024);
    int read = 0;
    for (int i = 0; i < 10 && read == 0; i++)
        read = tcpSocket->readFromSocket(buffer, 1023); // read from socket
    if (read == 0)
        return NULL;

    stringstream iss(buffer); // stringstream for parsing buffer
    string method = "";
    getline(iss, method, ' '); // extract whatever before a white space into method
                               // Invoke the httpRequestFactory for an httpRequest object of type equivelant to the request method
    HTTPRequest *httpRequest = httpRequestManager->getService(tcpSocket, method);
    // if an object is returned then execute the readAndParse method else an exception will be thrown
    if (httpRequest != NULL)
        httpRequest->readAndParse(buffer);
    return httpRequest; // return the httpRequest object
}

/**
 * @brief Processes the HTTP transaction.
 *
 * This function fetches an HTTP request, executes the corresponding HTTP service, and handles exceptions.
 * It initializes an HTTP request, response, and service object. It then attempts to fetch the HTTP request,
 * execute the service, and handle any exceptions that may occur.
 *
 * @note The function deletes the HTTP request, response, and service objects after processing.
 * @note The function also shuts down the TCP socket after processing.
 *
 * @return void
 * @authors Kmsobh, Ramy
 * @date 14-Aug-2024
 */
void HTTPTransaction::process()
{
    HTTPService *s;
    HTTPRequest *httpRequest; // Define a pointer to an HTTPRequest object
    HTTPResponse *httpResponse = new HTTPResponse(tcpSocket);

    try
    {
        httpRequest = fetchHTTPRequest();
        // Invoke the HTTPServiceManager for a service that can serve the request resource
        if (httpRequest != NULL)
        {
            s = httpServiceManager->getService(httpRequest->getResource());
            s->execute(httpRequest, httpResponse, middlewareManager); // Execute the service
            delete (httpRequest);                                     // delete the httpRequest object
            delete (httpResponse);                                    // delete the httpResponse object
            delete (s);
        }
        tcpSocket->shutDown(); // Shutdown the TCP socket
    }
    catch (HTTPNotAcceptableExceptionHandler httpNotAcceptableExceptionHandler)
    {                                                           // An exception occurred indicating that the service requested is not accepted
        httpNotAcceptableExceptionHandler.handle(httpResponse); // handle exception
        delete (httpRequest);                                   // delete the httpRequest object
        tcpSocket->shutDown();                                  // Shutdown the TCP socket
    }
    catch (HTTPMethodNotAllowedExceptionHandler httpMethodNotAllowedExceptionHandler)
    {                                                              // An exception occurred indicating that the requested resource cannot be found or accessed
        httpMethodNotAllowedExceptionHandler.handle(httpResponse); // handle exception
        tcpSocket->shutDown();                                     // Shutdown the TCP socket
    }
    // This is where we catch any exception related to the HTTPBlnkService, we know for sure that any exception will be handled there because
    // the statement s->execute is the one that caused an HTTPBlnkService to be initialized in the first place, s and httpRequest are deleted to make
    // sure that the garbage collection is done in a right way, we know that s and httpRequest are not NULL because s->execute must have been called in the
    // try statement above.
    //  catch(HTTPBlnkServiceNotAcceptableExceptionHandler httpBlnkServiceNotAcceptableExceptionHandler){
    //      httpBlnkServiceNotAcceptableExceptionHandler.handle(tcpSocket);
    //      delete(httpRequest);
    //      delete(s);
    //      tcpSocket->shutDown();
    //      // class defition should be made with a field for the error to be made
    //      //handle the exception here
    //  }
}
// Main thread body entry point
void HTTPTransaction::threadMainBody()
{
    process(); // invoke process
    th->detach();
}
void HTTPTransaction::startHTTPTransaction(HTTPTransaction *me)
{
    me->threadMainBody();
}
void HTTPTransaction::setThread(std::thread *p_th)
{
    th = p_th;
}
bool HTTPTransaction::isRunning()
{
    return th->joinable();
}

void HTTPTransaction::waitForRunToFinish()
{
    if (this->isRunning())
        th->join();
}
// Destructor
HTTPTransaction::~HTTPTransaction()
{
    delete (tcpSocket); // Delete the tcpSocket object
    delete (th);
}
