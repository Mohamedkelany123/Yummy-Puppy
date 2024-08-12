#ifndef SharedObjectsManager_H
#define SharedObjectsManager_H

#include "includes.h"
#include "HTTPService.h"
#include "Middleware.h"

typedef HTTPService * create_object_routine();
typedef Middleware * create_middleware_routine();
class SharedObjectsManager{
    protected:
        map <string,HTTPService *> gobj; // A vector to store the cloner objects
        map <string, Middleware *> middles; // A vector to store all middleware objects
        map <string,void *> dsos; // A vector to store all DSO objects

    public:
        SharedObjectsManager(); 
	    HTTPService * load(string name ); // Loading a DSO from file
        Middleware * loadMiddleware(string name); // Loading a DSO from file
	    ~SharedObjectsManager();
};

#endif