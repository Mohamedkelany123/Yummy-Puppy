#include "Middleware.h"

Middleware::Middleware()
{
}
Middleware::Middleware(const string _middlewareName) : Middleware()
{
    this->middlewareName = _middlewareName;
}

string Middleware::getName()
{
    return this->middlewareName;
}

void Middleware::setParams(json _params)
{
    params = _params;
}

json Middleware::getParams()
{
    return params;
}

Middleware::~Middleware()
{
}
