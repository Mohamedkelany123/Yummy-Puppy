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
            for(auto& middlewarePath : middlewaresPaths){
                for (auto& path : middlewarePath.items() ) { 
                    if(ends_with(path.value(), ".so")){
                        this->middlewares[path.key()] = sharedObjectPtr->loadMiddleware(path.value());
                    }
                }
            }
        }
        continue;
    }
}