#ifndef _MIDDLEWARE_H_
#define _MIDDLEWARE_H_
#include "HTTPRequest.h"
#include "HTTPResponse.h"



class  Middleware
{
    private:
    const HTTPRequest* req;
    const HTTPResponse* res;
    public:
        Middleware(const HTTPRequest* req, const HTTPResponse* res);
        virtual bool pre() = 0;
        virtual bool post()=0;
        void execute();
        virtual ~Middleware();


};


#endif
