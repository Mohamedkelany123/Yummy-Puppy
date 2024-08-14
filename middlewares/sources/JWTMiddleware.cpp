#include <JWTMiddleware.h>

JWTMiddleware::JWTMiddleware() : Middleware("JWT")
{
}
bool JWTMiddleware::run(HTTPRequest *_req, HTTPResponse *_res)
{
    if (_req != nullptr)
    {
        string str = _req->getHeaderValue("Accept");
        cout << _req->getHeaderValue("Authorization") << endl;
    }
    cout << "This is JWTMiddleware::run()" << endl;
    return true;
}
Middleware *JWTMiddleware::clone()
{
    return (Middleware *)new JWTMiddleware();
}

JWTMiddleware::~JWTMiddleware()
{
}

extern "C" Middleware *create_object() // extern "c" not garbling function names
{
    return new JWTMiddleware();
}
