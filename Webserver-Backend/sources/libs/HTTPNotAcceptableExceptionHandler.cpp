#include "HTTPNotAcceptableExceptionHandler.h"
#include "HTTPResponseHeader.h"
// Constructor
HTTPNotAcceptableExceptionHandler::HTTPNotAcceptableExceptionHandler(){

}
HTTPNotAcceptableExceptionHandler::HTTPNotAcceptableExceptionHandler(int except_no_param){
    except_no = except_no_param;
}
// Handling the unacceptable request exception: HTTP status code 406
void HTTPNotAcceptableExceptionHandler::handle (HTTPResponse* response)
{
    json reply;
    reply["error"] = "Error Code: "+to_string(except_no);
    reply["status_code"] = 406;
    response->getHeader()->setStatus("Not Acceptable");
    response->getHeader()->setStatusCode(406);
    response->getHeader()->setProtocol("HTTP/1.1");
    response->setHeaderValue("Content-Type", "application/json");
    response->setBody(reply);
    response->write();

}
// Destructor
HTTPNotAcceptableExceptionHandler::~HTTPNotAcceptableExceptionHandler()
{
    
}
