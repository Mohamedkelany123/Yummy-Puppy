#include "HTTPMethodNotAllowedExceptionHandler.h"
#include "HTTPResponseHeader.h"
// Constructor
HTTPMethodNotAllowedExceptionHandler::HTTPMethodNotAllowedExceptionHandler(){}

// Handling the unallowed request exception: HTTP status code 405
void HTTPMethodNotAllowedExceptionHandler::handle (TCPSocket * p_tcpSocket)
{
    // String to hold response body
    string response = "<H1>405: Method Not Allowed</H1>";
    // Instantiate an HTTPResponseHeader object and set its header attributes    
    HTTPResponseHeader * httpResponseHeader = new HTTPResponseHeader(p_tcpSocket,"Method Not Allowed",405,"HTTP/1.1");
    httpResponseHeader->setHeader("Content-Type","text/html");
    httpResponseHeader->setHeader("Content-Length",to_string(response.length()));
    // Respond to client by sending the response header on the p_tcpSocket
    httpResponseHeader->respond();
    // Write the body string to the client via p_tcpSockey    
    p_tcpSocket->writeToSocket(response.c_str(),response.length());
    delete (httpResponseHeader); // Destruct the HTTPResponseHeader object

}
// Destructor
HTTPMethodNotAllowedExceptionHandler::~HTTPMethodNotAllowedExceptionHandler(){}