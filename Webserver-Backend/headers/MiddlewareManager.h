#ifndef _MIDDLEWAREMANAGER_H_
#define _MIDDLEWAREMANAGER_H_

#include "SharedObjectsManager.h"
#include "ConfigFile.h"
#include "Middleware.h"

class MiddlewareManager{
    private:
        std::map <string,Middleware *> middlewares;
        std::map <string,vector<Middleware *>> endpointsMiddlewares;
        SharedObjectsManager * sharedObjectPtr;
    public:
        MiddlewareManager(ConfigFile * conf, Logger* logger); // Constructor
        // Return a service whose type depends on the requested resource.
        void assignEndpoint(string name, vector<string> middleware_list);
        vector<Middleware *> * getEndpointMiddlewares(string name);
        ~MiddlewareManager(); // Destructor
};

#endif