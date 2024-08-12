#ifndef _JWTMIDDLEWARE_H_
#define _JWTMIDDLEWARE_H_
#include "Middleware.h"



class  JWTMiddleware: public Middleware
{
    private:

    public:
        JWTMiddleware ();
        JWTMiddleware(const HTTPRequest* _req, const HTTPResponse* _res);
        virtual bool pre();
        virtual bool post();
        virtual Middleware * clone();
        virtual ~JWTMiddleware();
};


#endif
