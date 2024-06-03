#include "HTTPNotAcceptableExceptionHandler.h"
#include "HTTPResponseHeader.h"
// Constructor
HTTPNotAcceptableExceptionHandler::HTTPNotAcceptableExceptionHandler(){

}
HTTPNotAcceptableExceptionHandler::HTTPNotAcceptableExceptionHandler(int except_no_param){
    except_no = except_no_param;
}
// Handling the unacceptable request exception: HTTP status code 406
void HTTPNotAcceptableExceptionHandler::handle (TCPSocket * p_tcpSocket)
{
    // String to hold response body
    string response ="<H1>Error Code: " +to_string(except_no) + "</H1>";
    // Instantiate an HTTPResponseHeader object and set its header attributes
    HTTPResponseHeader * httpResponseHeader = new HTTPResponseHeader(p_tcpSocket,"Not Acceptable",406,"HTTP/1.1"); 
    httpResponseHeader->setHeader("Content-Type","text/html");
    httpResponseHeader->setHeader("Content-Length",to_string(response.length()));
    // Respond to client by sending the response header on the p_tcpSocket
    httpResponseHeader->respond();
    // Write the body string to the client via p_tcpSockey
    p_tcpSocket->writeToSocket(response.c_str(),response.length());
    delete (httpResponseHeader); // Destruct the HTTPResponseHeader object

}
// Destructor
HTTPNotAcceptableExceptionHandler::~HTTPNotAcceptableExceptionHandler()
{
    
}
