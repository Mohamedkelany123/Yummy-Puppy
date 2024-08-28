#ifndef HTTPOPTIONSREQUEST_H
#define HTTPOPTIONSREQUEST_H

#include "HTTPRequest.h"
#include "TCPSocket.h"

class HTTPOPTIONSRequest : public HTTPRequest {
public:

    HTTPOPTIONSRequest(TCPSocket * p_tcpSocket);
    virtual HTTPRequest * clone(TCPSocket * p_tcpSocket) override;
    virtual ~HTTPOPTIONSRequest();

};

#endif
