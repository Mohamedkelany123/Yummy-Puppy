#include "HTTPNotFoundExceptionHandler.h"
#include "HTTPResponseHeader.h"
// Constructor
HTTPNotFoundExceptionHandler::HTTPNotFoundExceptionHandler(){}
// Handling the resource not found exception: HTTP status code 404
void HTTPNotFoundExceptionHandler::handle (HTTPResponse* response)
{
    json reply;
    reply["error"] = "Not Found";
    reply["status_code"] = 404;
    response->getHeader()->setStatus("Not Found");
    response->getHeader()->setStatusCode(404);
    response->getHeader()->setProtocol("HTTP/1.1");
    response->setHeaderValue("Content-Type", "application/json");
    response->setBody(reply);
    response->write();

}
// Destructor
HTTPNotFoundExceptionHandler::~HTTPNotFoundExceptionHandler(){}
