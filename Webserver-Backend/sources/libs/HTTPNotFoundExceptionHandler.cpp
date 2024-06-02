#include "HTTPNotFoundExceptionHandler.h"
#include "HTTPResponseHeader.h"
// Constructor
HTTPNotFoundExceptionHandler::HTTPNotFoundExceptionHandler(){}
// Handling the resource not found exception: HTTP status code 404
void HTTPNotFoundExceptionHandler::handle (TCPSocket * p_tcpSocket)
{
    // String to hold response body
    string response = "<H1>404: File Not Found</H1>";
    // Instantiate an HTTPResponseHeader object and set its header attributes    
    HTTPResponseHeader * httpResponseHeader = new HTTPResponseHeader(p_tcpSocket,"Not Found",404,"HTTP/1.1");
    httpResponseHeader->setHeader("Content-Type","text/html");
    httpResponseHeader->setHeader("Content-Length",to_string(response.length()));
    // Respond to client by sending the response header on the p_tcpSocket
    httpResponseHeader->respond();
    // Write the body string to the client via p_tcpSockey    
    p_tcpSocket->writeToSocket(response.c_str(),response.length());
    delete (httpResponseHeader); // Destruct the HTTPResponseHeader object

}
// Destructor
HTTPNotFoundExceptionHandler::~HTTPNotFoundExceptionHandler(){}
