#include "HTTPMethodNotAllowedExceptionHandler.h"
#include "HTTPResponse.h"
// Constructor
HTTPMethodNotAllowedExceptionHandler::HTTPMethodNotAllowedExceptionHandler(){}

// Handling the unallowed request exception: HTTP status code 405
void HTTPMethodNotAllowedExceptionHandler::handle (HTTPResponse* response)
{
    // String to hold response body
    json reply;
    reply["error"] = "Method Not Allowed";
    reply["status_code"] = 405;
    // Instantiate an HTTPResponseHeader object and set its header attributes   
    response->getHeader()->setStatus("Method Not Allowed");
    response->getHeader()->setStatusCode(405);
    response->getHeader()->setProtocol("HTTP/1.1");
    response->setHeaderValue("Content-Type", "application/json");
    response->setBody(reply);
    response->write();
    // Write the body string to the client via p_tcpSockey    
    
}
// Destructor
HTTPMethodNotAllowedExceptionHandler::~HTTPMethodNotAllowedExceptionHandler(){}