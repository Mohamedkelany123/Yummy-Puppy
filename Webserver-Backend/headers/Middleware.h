#ifndef _MIDDLEWARE_H_
#define _MIDDLEWARE_H_
#include "HTTPRequest.h"
#include "HTTPResponse.h"



class  Middleware
{
    protected:
        const HTTPRequest* req;
        const HTTPResponse* res;
    public:
        Middleware();
        Middleware(const HTTPRequest* _req, const HTTPResponse* _res);
        virtual bool pre() = 0;
        virtual bool post()=0;
        virtual Middleware * clone() =0;
        virtual ~Middleware();
};


#endif
