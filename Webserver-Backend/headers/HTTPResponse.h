#ifndef _HTTPRESPONSE_H_
#define _HTTPRESPONSE_H_
#include "HTTPResponseHeader.h"
#include "TCPSocket.h"


class HTTPResponse
{

    private:
        HTTPResponseHeader * httpResponseHeader;
        json body;
        TCPSocket * tcpSocket;
    public:
        HTTPResponse(TCPSocket * _tcpSocket);
        void setBody(json _body);
        void write();
        void setHeaderValue(string _header, string _value);
        void appendBody(json _body);
        template <typename T>
        void setBodyValue(string _key, T _value){
            body[_key] = _value;
        }
        json getBodyValue(string _key);
        HTTPResponseHeader * getHeader();

        ~HTTPResponse();
};


#endif