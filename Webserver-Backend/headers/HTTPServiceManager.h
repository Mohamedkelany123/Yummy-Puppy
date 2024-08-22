#ifndef HTTPSERVICEMANAGER_H
#define HTTPSERVICEMANAGER_H

#include "SharedObjectsManager.h"
#include "ConfigFile.h"
#include "MiddlewareManager.h"
#include "HTTPService.h"
#include <URLService.h>


class HTTPServiceManager // A class for managing services and acting as a simple object factory
{
    private:
        std::map <string,HTTPService *> services; // A map that relate service identifier to corresponding pointers to HTTPService cloners
        map<string,vector<string> *> servicesParameters;
        SharedObjectsManager<HTTPService> * sharedObjectPtr;
        vector<string> regexURLs;
    public:
        HTTPServiceManager(ConfigFile * conf, Logger* logger,MiddlewareManager * _middlewareManager); // Constructor
        HTTPService * getService (string p_resource);
        vector<string> getURLParams(string URLPath);
        vector<string> getServiceParams(string _key);
        map<string, string> extractURLParams(string _url);
        string getRegexURL(string URLPath);

        ~HTTPServiceManager(); // Destructor
};


#endif // HTTPSERVICEMANAGER_H