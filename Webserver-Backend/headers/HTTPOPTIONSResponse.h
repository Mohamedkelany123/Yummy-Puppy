#ifndef HTTPOPTIONSRESPONSE_H
#define HTTPOPTIONSRESPONSE_H

#include "HTTPResponse.h"
#include "TCPSocket.h"

class HTTPOPTIONSResponse : public HTTPResponse {
    public:
        HTTPOPTIONSResponse(TCPSocket * _tcpSocket);
        virtual ~HTTPOPTIONSResponse();

};

#endif
