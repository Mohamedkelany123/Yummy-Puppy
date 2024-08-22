#include "HTTPServiceManager.h"
#include "HTTPNotAcceptableExceptionHandler.h"
#include <regex>

#define WEB_CACHE_ROOT  "./www"
// Constructor:  building up the factory map
HTTPServiceManager::HTTPServiceManager(ConfigFile * conf, Logger* logger,MiddlewareManager * _middlewareManager)
{
    sharedObjectPtr = new SharedObjectsManager<HTTPService>();
    for (auto& el : conf->data.items()) {
        if(el.key() == "server_config")
            continue;
        std::cout << el.key() << " : ";
        try{
            string so_path = el.value()["so_path"];
            string http_path = el.value()["http_path"];
            vector<string> parameters = URLService::getURLParams(http_path);
            http_path = URLService::getRegexURL(http_path);
            servicesParameters[http_path] = &parameters;

            auto middlewares = el.value()["middlewares"];
            auto preMiddlewares = el.value()["middlewares"]["preMiddlewares"];
            auto postMiddlewares = el.value()["middlewares"]["postMiddlewares"];

            vector<string> endpointPreMiddlewares;
            for (auto& el : preMiddlewares.items()) {
                cout << el.key() << ": " << el.value() << endl;
                endpointPreMiddlewares.push_back(el.value());
            }

            vector<string> endpointPostMiddlewares;
            for (auto& el : postMiddlewares.items()) {
                cout << el.key() << ": " << el.value() << endl;
                endpointPostMiddlewares.push_back(el.value());
            }
            cout << "SO_Path: " << so_path << endl;
            cout << "HTTP_Path: " << http_path << endl;
            cout << "KEY: "<< el.key() << endl;
    //        services [el.key()] = sharedObjectPtr->load(so_path);
            services [http_path] = sharedObjectPtr->load(so_path);
            _middlewareManager->assignEndpointPreMiddlewares(http_path, endpointPreMiddlewares);
            _middlewareManager->assignEndpointPostMiddlewares(http_path, endpointPostMiddlewares);
            regexURLs.push_back(http_path);
        }catch(exception e){
            LOG_ERRORS(e.what());
        }
    }
}
// Compare the file extention to the map key first and if not found compare the whole file name
HTTPService * HTTPServiceManager::getService (string p_resource)
{
    // extract extentions
    cout << "p_resource: " << p_resource << endl;
    string ext = p_resource;//.substr(p_resource.find_last_of(".") + 1);
    pair<string, HTTPService *>  service = URLService::searchRegexMapWithKey(p_resource, &services);
    if(service.second == nullptr){
        throw (HTTPNotAcceptableExceptionHandler());
    }
    else return service.second->clone(); // clone service based on extension
}

map<string, string> HTTPServiceManager::extractURLParams(string _url)
{
    pair<string, vector<string> *> parameters = URLService::searchRegexMapWithKey(_url, &servicesParameters);
    if(parameters.second->size() == 0) return map<string, string>();
    string regexURL = parameters.first;

    vector<string> _urlSplit = URLService::splitURL(_url);
    vector<string> regexURLSplit = URLService::splitURL(regexURL);
    map<string, string> parametersValues;

    int asteriskCount = 0;

    for (int i = 0; i < regexURLSplit.size(); i++){
        if(regexURLSplit[i] == "*"){
            parametersValues[parameters.second->operator[](asteriskCount)] = _urlSplit[i];
            asteriskCount++;
        }
    }
    return parametersValues;
}
// Destructor
HTTPServiceManager::~HTTPServiceManager()
{
    // A for_each iterator based loop with lambda function to deallocate all the cloner objects
    for_each (services.begin(),services.end(),[](const std::pair<string,HTTPService *>& it) -> bool {
        HTTPService * httpService = std::get<1>(it);
        delete(httpService);
        return true;
   });
   for(auto m : this->servicesParameters)
        delete(m.second);
}