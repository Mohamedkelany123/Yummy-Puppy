#ifndef _PSQLINITMIDDLEWARE_H_
#define _PSQLINITMIDDLEWARE_H_
#include <Middleware.h>


class  PSQLInitMiddleware: public Middleware
{
    private:
    public:
        PSQLInitMiddleware ();
        virtual bool run(HTTPRequest* _req, HTTPResponse* _res);
        virtual Middleware * clone();
        void init(json initData);
        void setupDataSources();
        virtual ~PSQLInitMiddleware();
};


#endif
