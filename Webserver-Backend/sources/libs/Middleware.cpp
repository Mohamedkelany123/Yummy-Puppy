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

Middleware::~Middleware()
{
}
