#include "Middleware.h"

Middleware::Middleware(const HTTPRequest *req, const HTTPResponse *res)
{
    this->req = req;
    this->res = res;
}

void Middleware::execute()
{
}
