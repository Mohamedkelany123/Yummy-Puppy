#ifndef _JWTMIDDLEWARE_H_
#define _JWTMIDDLEWARE_H_
#include <Middleware.h>
#include <Poco/JWT/JWT.h>
#include <Poco/JWT/Signer.h>
#include <Poco/JWT/Token.h>
#include <Poco/Exception.h>


#include <PSQLConnection.h>
#include <PSQLQuery.h>

using namespace Poco::JWT;


class  JWTMiddleware: public Middleware
{
    private:
        PSQLConnection * connection;
        Token token;
    public:
        JWTMiddleware ();
        virtual bool run(HTTPRequest* _req, HTTPResponse* _res);
        virtual Middleware * clone();
        void connectDatabase();
        void init(json initData);
        PSQLConnection * getDatabaseConnection();
        pair<string *, bool> verifyToken(string authToken);
        bool verifyUser(string userID);
        void injectUserData(HTTPRequest* _req, map<string, string> userID);
        virtual ~JWTMiddleware();
};


#endif
