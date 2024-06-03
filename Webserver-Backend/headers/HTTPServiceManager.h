#ifndef HTTPSERVICEMANAGER_H
#define HTTPSERVICEMANAGER_H

#include "SharedObjectsManager.h"
#include "ConfigFile.h"

class HTTPServiceManager // A class for managing services and acting as a simple object factory
{
    private:
        std::map <string,HTTPService *> services; // A map that relate service identifier to corresponding pointers to HTTPService cloners
        SharedObjectsManager * sharedObjectPtr;
    public:
        HTTPServiceManager(ConfigFile * conf, Logger* logger); // Constructor
        // Return a service whose type depends on the requested resource.
        HTTPService * getService (string p_resource);
        ~HTTPServiceManager(); // Destructor
};


#endif // HTTPSERVICEMANAGER_H