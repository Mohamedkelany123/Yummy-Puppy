#include "HTTPBadRequestExceptionHandler.h"
#include "HTTPResponseHeader.h"
// Constructor
HTTPBadRequestExceptionHandler::HTTPBadRequestExceptionHandler(){

}
HTTPBadRequestExceptionHandler::HTTPBadRequestExceptionHandler(string error){
    error_messege = error;
}
// Handling the bad request request exception: HTTP status code 400
void HTTPBadRequestExceptionHandler::handle (HTTPResponse* response)
{
    json reply;
    reply["error"] = "Error : " + error_messege;
    reply["status_code"] = 400;
    response->getHeader()->setStatus("Bad Request");
    response->getHeader()->setStatusCode(400);
    response->getHeader()->setProtocol("HTTP/1.1");
    response->setHeaderValue("Content-Type", "application/json");
    response->setBody(reply);
    response->write();

}
void HTTPBadRequestExceptionHandler::setErrorMessage(string message){
    error_messege = message;
}

// Destructor
HTTPBadRequestExceptionHandler::~HTTPBadRequestExceptionHandler()
{
    
}
