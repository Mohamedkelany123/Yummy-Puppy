#include "HTTPTransaction.h"
#include "HTTPGETRequest.h"
#include "HTTPNotAcceptableExceptionHandler.h"
#include "HTTPMethodNotAllowedExceptionHandler.h"
#include "HTTPBadRequestExceptionHandler.h"
#include <HTTPOPTIONSResponse.h>
#include <AbstractException.h>


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
        httpRequest->readAndParse(buffer,read,buffer);
    return httpRequest; // return the httpRequest object
}


/**
 * @brief Processes the HTTP transaction by fetching the request, executing the corresponding service, and handling exceptions.
 *
 * This function performs the following tasks:
 * 1. Fetches the HTTP request using the fetchHTTPRequest method.
 * 2. Determines the appropriate HTTP service based on the requested resource using the HTTPServiceManager.
 * 3. Extracts URL parameters from the requested resource and adds them to the HTTP request context.
 * 4. If the request method is "OPTIONS", it responds with an HTTP OPTIONS response.
 * 5. Executes the determined HTTP service with the HTTP request, response, and middleware manager.
 * 6. Catches and handles exceptions related to bad requests, unacceptable services, and method not allowed.
 * 7. Shuts down the TCP socket after processing the request.
 *
 * @note This function assumes that the HTTPServiceManager, httpRequestManager, and middlewareManager are properly initialized.
 * @note The function deletes the HTTP response, request, and service objects after processing to ensure proper memory management.
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
            map<string, string> * URLParamters = httpServiceManager->extractURLParams(httpRequest->getResource());
            map<string, string> * queryParamters = httpServiceManager->extractURLQueryParams(httpRequest->getResource());
            httpRequest->addContext("url_params", *URLParamters);
            httpRequest->addContext("query_params", *queryParamters);
            if(httpRequest->getMethod() == "OPTIONS"){
                HTTPOPTIONSResponse *httpOPTIONSResponse = new HTTPOPTIONSResponse(tcpSocket);
                httpOPTIONSResponse->write();
                delete(httpOPTIONSResponse);
            }else
                s->execute(httpRequest, httpResponse, middlewareManager); // Execute the service
            delete (httpResponse);                                    // delete the httpResponse object
            delete (httpRequest);                                     // delete the httpRequest object
            delete (s);
        }
    }
    catch (BadRequest e)
    {       
        HTTPBadRequestExceptionHandler httpBadRequestExceptionHandler;  
        httpBadRequestExceptionHandler.setErrorMessage(e.getMessege());
        httpBadRequestExceptionHandler.handle(httpResponse); // handle exception
        delete (httpRequest); // delete the httpRequest object
    }
    catch (HTTPNotAcceptableExceptionHandler httpNotAcceptableExceptionHandler)
    {                                                           // An exception occurred indicating that the service requested is not accepted
        httpNotAcceptableExceptionHandler.handle(httpResponse); // handle exception
        delete (httpRequest);                                   // delete the httpRequest object
    }
    catch (HTTPMethodNotAllowedExceptionHandler httpMethodNotAllowedExceptionHandler)
    {                                                              // An exception occurred indicating that the requested resource cannot be found or accessed
        httpMethodNotAllowedExceptionHandler.handle(httpResponse); // handle exception
    }
    tcpSocket->shutDown();                                     // Shutdown the TCP socket
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
    cout << "Starting transaction with Thread ID:" << thread::get_id() << endl;
    me->threadMainBody();
}
void HTTPTransaction::setThread(TeamThread *p_th)
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
