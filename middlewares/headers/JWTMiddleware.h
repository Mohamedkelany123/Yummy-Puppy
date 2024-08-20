#ifndef _JWTMIDDLEWARE_H_
#define _JWTMIDDLEWARE_H_
#include <Middleware.h>
#include <Poco/JWT/JWT.h>
#include <Poco/JWT/Signer.h>
#include <Poco/JWT/Token.h>

using namespace Poco::JWT;


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
