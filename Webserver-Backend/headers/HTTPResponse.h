#ifndef _HTTPRESPONSE_H_
#define _HTTPRESPONSE_H_
#include "HTTPResponseHeader.h"


class HTTPResponse
{

    private:
        HTTPResponseHeader * httpResponseHeader;
        string body;
    public:
        HTTPResponse(HTTPResponseHeader * _httpResponseHeader);
        void setBody(string _body);
        void setBody(json _body);
        ~HTTPResponse();
};


#endif