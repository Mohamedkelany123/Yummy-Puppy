#include <JWTMiddleware.h>


JWTMiddleware::JWTMiddleware():Middleware()
{

}

JWTMiddleware::JWTMiddleware(const HTTPRequest* _req, const HTTPResponse* _res):Middleware(_req,_res)
{

}
bool JWTMiddleware::pre()
{
    cout << "This is JWTMiddleware::pre()" << endl;
    return true;
}
bool JWTMiddleware::post()
{
    cout << "This is JWTMiddleware::post()" << endl;
    return true;

}
Middleware * JWTMiddleware::clone()
{
    return (Middleware *) new JWTMiddleware();
}

JWTMiddleware::~JWTMiddleware()
{

}


extern "C" Middleware *create_object() // extern "c" not garbling function names
{

    return new JWTMiddleware ();

}
