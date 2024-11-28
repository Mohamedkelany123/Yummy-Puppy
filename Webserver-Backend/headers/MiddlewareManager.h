#ifndef _MIDDLEWAREMANAGER_H_
#define _MIDDLEWAREMANAGER_H_

#include "ConfigFile.h"
#include "Middleware.h"
#include "Logger.h"
#include "SharedObjectsManager.h"
#include "URLService.h"
#include <ResourceManager.h>
#include <mutex>



class MiddleWareResourceManager: public ResourceManager<Middleware>
{
    private:
        Middleware * cloner;
    public:
        MiddleWareResourceManager (Middleware * _cloner):ResourceManager <Middleware>([](ResourceManager <Middleware> * me)->Middleware* {
            MiddleWareResourceManager * mee = (MiddleWareResourceManager *)me;
            cout << "This is the cloner of the middleware" << endl;
            return mee->cloner->clone();
        })
        {
                cloner=_cloner;
        }
        ~MiddleWareResourceManager(){}        

};

class MiddlewareManager{
    private:
        std::map <string,Middleware *> middlewares;
        std::map <string,MiddleWareResourceManager *> middleWareResourceManager;
        std::map <string,vector<MiddleWareResourceManager *> *> endpointsPreMiddlewares;
        std::map <string,vector<MiddleWareResourceManager *> *> endpointsPostMiddlewares;
        SharedObjectsManager<Middleware> * sharedObjectPtr;
        Logger * logger;
        mutex lock;
    public:
        MiddlewareManager(ConfigFile * conf, Logger* logger); // Constructor
        // Return a service whose type depends on the requested resource.
        void assignEndpointPreMiddlewares(string name, vector<string> middleware_list);
        void assignEndpointPostMiddlewares(string name, vector<string> middleware_list);
        bool runEndpointPreMiddleware(string endpointName, HTTPRequest *req, HTTPResponse *res);
        bool runEndpointPostMiddleware(string endpointName, HTTPRequest *req, HTTPResponse *res);
        bool runMiddlewares(vector<MiddleWareResourceManager *> middlewaresList, HTTPRequest *req, HTTPResponse *res);
        vector<Middleware *> * getEndpointMiddlewares(string name);
        void deleteEndpointMiddleware(vector<MiddleWareResourceManager *> middlewareManagersList,vector<Middleware *> localMiddleware);
        ~MiddlewareManager(); // Destructor
};

#endif