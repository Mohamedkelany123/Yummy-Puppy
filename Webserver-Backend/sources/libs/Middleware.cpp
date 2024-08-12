#include "Middleware.h"


Middleware::Middleware()
{
    this->req = NULL;
    this->res = NULL;

}
Middleware::Middleware(const HTTPRequest *_req, const HTTPResponse *_res):Middleware()
{
    this->req = _req;
    this->res = _res;
}
Middleware::~Middleware()
{
}


