#ifndef _MIDDLEWARE_H_
#define _MIDDLEWARE_H_
#include "HTTPRequest.h"
#include "HTTPResponse.h"



class  Middleware
{
    protected:
        string middlewareName;
    public:
        Middleware();
        Middleware(const string _middlewarename);
        virtual bool run(HTTPRequest* _req, HTTPResponse* _res)=0;
        virtual Middleware * clone() =0;
        string getName();
        virtual ~Middleware();
};


#endif
