#ifndef _JWTMIDDLEWARE_H_
#define _JWTMIDDLEWARE_H_
#include "Middleware.h"



class  JWTMiddleware: public Middleware
{
    private:

    public:
        JWTMiddleware ();
        virtual bool run(HTTPRequest* _req, HTTPResponse* _res);
        virtual Middleware * clone();
        virtual ~JWTMiddleware();
};


#endif
