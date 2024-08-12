#ifndef _MIDDLEWAREMANAGER_H_
#define _MIDDLEWAREMANAGER_H_

#include "SharedObjectsManager.h"
#include "ConfigFile.h"
#include "Middleware.h"

class MiddlewareManager{
    private:
        std::map <string,Middleware *> middlewares;
        std::map <string,vector<string>> endpointsMiddlewares;
        SharedObjectsManager * sharedObjectPtr;
    public:
        MiddlewareManager(ConfigFile * conf, Logger* logger); // Constructor
        // Return a service whose type depends on the requested resource.

        ~MiddlewareManager(); // Destructor
};

#endif