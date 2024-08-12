#include "MiddlewareManager.h"

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}
MiddlewareManager::MiddlewareManager(ConfigFile * conf, Logger* logger){
    sharedObjectPtr = new SharedObjectsManager();
    for (auto& el : conf->data.items()) {
        if(el.key() == "server_config"){
            auto middlewaresPaths = el.value()["middlewares"];
           for (auto& el : middlewaresPaths.items()) {
               for (auto& el1 :el.value().items()) {
                    std::cout << el1.key() <<  ":" <<  el1.value()["DSO"] << "\n";
                    this->middlewares[el1.key()] = sharedObjectPtr->loadMiddleware(el1.value()["DSO"]);
                    break;
               }
            }
            // for(auto& middlewarePath : middlewaresPaths){
            //     for (auto& path : middlewarePath.items() ) { 
            //         if(ends_with(path.value(), ".so")){
            //             this->middlewares[path.key()] = sharedObjectPtr->loadMiddleware(path.value());
            //         }
            //     }
            // }
        }
        continue;
    }
}
void MiddlewareManager::assignEndpoint(string name, vector<string> middleware_list){
    for (int i  = 0 ; i < middleware_list.size() ; i ++)
    {
            endpointsMiddlewares[name].push_back(middlewares[middleware_list[i]]);
    }
}

vector<Middleware *> * MiddlewareManager::getEndpointMiddlewares(string name)
{
    return &(endpointsMiddlewares[name]);
}

MiddlewareManager::~MiddlewareManager()
{
    delete (sharedObjectPtr);
    for ( auto m: this->middlewares)
        delete (m.second);

}