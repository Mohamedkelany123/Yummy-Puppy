#include "Middleware.h"

Middleware::Middleware()
{
}
Middleware::Middleware(const string _middlewareName) : Middleware()
{
    this->middlewareName = _middlewareName;
}

void Middleware::init(json initData)
{
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

json Middleware::getParamValue(string _key)
{
    if(!params.contains(_key)) return NULL;
    return params[_key];
}

Middleware::~Middleware()
{
}
