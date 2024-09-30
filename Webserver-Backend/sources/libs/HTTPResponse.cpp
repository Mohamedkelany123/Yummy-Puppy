#include <HTTPResponse.h>


HTTPResponse::HTTPResponse(TCPSocket * _tcpSocket)
{
    HTTPResponseHeader * _httpResponseHeader = new HTTPResponseHeader("OK",200,"HTTP/1.1");
    _httpResponseHeader->setHeader("Content-Type","application/json");
    _httpResponseHeader->setHeader("charset","utf-8");
    _httpResponseHeader->setHeader("Access-Control-Allow-Origin", "*");
    _httpResponseHeader->setHeader("Access-Control-Allow-Methods", "*");
    _httpResponseHeader->setHeader("Access-control-allow-credentials", "true");
    _httpResponseHeader->setHeader("Access-Control-Allow-Headers", "Content-Type");
    httpResponseHeader = _httpResponseHeader;
    tcpSocket = _tcpSocket;
}

void HTTPResponse::setBody(json _body)
{
    body = _body;
}
void HTTPResponse::appendBody(json _body){
    body.merge_patch(_body);
}

json HTTPResponse::getBodyValue(string _key){
    return body[_key];
}
void HTTPResponse::write(){
    string reply = body.dump(0);
    httpResponseHeader->setHeader("Connection","close"); 
    httpResponseHeader->setHeader("Content-Length",to_string(reply.length()));
    httpResponseHeader->respond(tcpSocket);

    tcpSocket->writeToSocket(reply.c_str(),reply.length());
}

void HTTPResponse::setHeaderValue(string _header, string _value){
    httpResponseHeader->setHeader(_header, _value);
}

HTTPResponseHeader * HTTPResponse::getHeader(){
    return httpResponseHeader;
}

HTTPResponse::~HTTPResponse()
{
    delete(httpResponseHeader);
}
