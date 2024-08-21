#ifndef _MIDDLEWAREMANAGER_H_
#define _MIDDLEWAREMANAGER_H_

#include "ConfigFile.h"
#include "Middleware.h"
#include "Logger.h"
#include "SharedObjectsManager.h"
#include "URLService.h"

class MiddlewareManager{
    private:
        std::map <string,Middleware *> middlewares;
        std::map <string,vector<Middleware *>> endpointsPreMiddlewares;
        std::map <string,vector<Middleware *>> endpointsPostMiddlewares;
        SharedObjectsManager<Middleware> * sharedObjectPtr;
        Logger * logger;
    public:
        MiddlewareManager(ConfigFile * conf, Logger* logger); // Constructor
        // Return a service whose type depends on the requested resource.
        void assignEndpointPreMiddlewares(string name, vector<string> middleware_list);
        void assignEndpointPostMiddlewares(string name, vector<string> middleware_list);
        bool runEndpointPreMiddleware(string endpointName, HTTPRequest *req, HTTPResponse *res);
        bool runEndpointPostMiddleware(string endpointName, HTTPRequest *req, HTTPResponse *res);
        bool runMiddlewares(vector<Middleware *> middlewaresList, HTTPRequest *req, HTTPResponse *res);
        vector<Middleware *> * getEndpointMiddlewares(string name);
        void deleteEndpointMiddleware(vector<Middleware *> localMiddleware);
        ~MiddlewareManager(); // Destructor
};

#endif