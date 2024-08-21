#include "MiddlewareManager.h"

inline bool ends_with(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}
/**
 * @brief Constructor for the MiddlewareManager class.
 *
 * This constructor initializes the MiddlewareManager object with a given ConfigFile and Logger.
 * It loads the middleware shared objects from the configuration file and stores them in a map.
 *
 * @param conf A pointer to the ConfigFile object containing the middleware configuration.
 * @param logger A pointer to the Logger object for logging errors and messages.
 *
 * @note The constructor assumes that the ConfigFile contains a "server_config" section with a "middlewares"
 * subsection, where each middleware is specified with a unique name and a "DSO" (Dynamic Shared Object) path.
 *
 * @see ConfigFile, Logger, SharedObjectsManager
 * @author Ramy
 * @date 14-Aug-2024
 */
MiddlewareManager::MiddlewareManager(ConfigFile *conf, Logger *logger)
{
    this->logger = logger;
    sharedObjectPtr = new SharedObjectsManager<Middleware>();
    for (auto &el : conf->data.items())
    {
        if (el.key() == "server_config")
        {
            auto middlewaresPaths = el.value()["middlewares"];
            for (auto &el : middlewaresPaths.items())
            {
                for (auto &el1 : el.value().items())
                {
                    std::cout << el1.key() << ":" << el1.value()["DSO"] << "\n";
                    this->middlewares[el1.key()] = sharedObjectPtr->load(el1.value()["DSO"]);
                    this->middlewares[el1.key()]->setParams(el1.value()); 
                    break;
                }
            }
        }
        continue;
    }
}

void MiddlewareManager::assignEndpointPreMiddlewares(string name, vector<string> middleware_list)
{
    for (int i = 0; i < middleware_list.size(); i++)
    {
        endpointsPreMiddlewares[name].push_back(middlewares[middleware_list[i]]);
    }
}

void MiddlewareManager::assignEndpointPostMiddlewares(string name, vector<string> middleware_list)
{
    for (int i = 0; i < middleware_list.size(); i++)
    {
        endpointsPostMiddlewares[name].push_back(middlewares[middleware_list[i]]);
    }
}

/**
 * @brief Executes a list of middleware objects.
 *
 * This function takes a vector of Middleware pointers, clones each middleware, and runs them sequentially.
 * If any middleware fails to execute (returns false), an error message is logged, the cloned middleware objects are deleted,
 * and the function returns false. If all middlewares execute successfully, the cloned middleware objects are deleted,
 * and the function returns true.
 *
 * @param middlewaresList A vector of Middleware pointers to be executed.
 * @return True if all middlewares were successfully executed, false otherwise.
 *
 * @note The function assumes ownership of the middleware objects and will delete them using the 'delete' operator.
 *
 * @see Middleware, deleteEndpointMiddleware
 * @author Ramy
 * @date 14-Aug-2024
 */
bool MiddlewareManager::runMiddlewares(vector<Middleware *> middlewaresList, HTTPRequest *req, HTTPResponse *res)
{
    vector<Middleware *> localMiddlewares;
    for (int i = 0; i < middlewaresList.size(); i++)
        localMiddlewares.push_back((middlewaresList[i])->clone());

    for (int i = 0; i < localMiddlewares.size(); i++)
        if (!localMiddlewares[i]->run(req, res))
        {
            string errorMsg = "Error running pre-middleware: " + localMiddlewares[i]->getName();
            logger->error(errorMsg);
            deleteEndpointMiddleware(localMiddlewares);
            return false;
        }
    deleteEndpointMiddleware(localMiddlewares);
    return true;
}

/**
 * @brief Deletes the middleware objects from the given vector.
 *
 * This function iterates through the provided vector of Middleware pointers and deletes each object.
 * It is used to clean up dynamically allocated middleware objects after they have been executed.
 *
 * @param localMiddlewares A vector of Middleware pointers to be deleted.
 *
 * @note The function assumes ownership of the middleware objects and will delete them using the 'delete' operator.
 *
 * @see Middleware
 * @author Ramy
 * @date 14-Aug-2024
 */
void MiddlewareManager::deleteEndpointMiddleware(vector<Middleware *> localMiddlewares)
{
    for (int i = 0; i < localMiddlewares.size(); i++)
        delete (localMiddlewares[i]);
}

/**
 * @brief Runs the pre-middlewares for a specific endpoint.
 *
 * This function retrieves the pre-middlewares associated with the given endpoint name from the
 * `endpointsPreMiddlewares` map and calls the `runMiddlewares` function to execute them.
 *
 * @param endpointName The name of the endpoint for which to run the pre-middlewares.
 * @return True if all pre-middlewares were successfully executed, false otherwise.
 *
 * @note The pre-middlewares are stored in the `endpointsPreMiddlewares` map, which is a map of endpoint names
 * to vectors of Middleware pointers.
 *
 * @see runMiddlewares
 * @author Ramy
 * @date 14-Aug-2024
 */
bool MiddlewareManager::runEndpointPreMiddleware(string endpointName, HTTPRequest *req, HTTPResponse *res)
{
    // vector<Middleware *> preMiddlewares = endpointsPreMiddlewares[endpointName];
    vector<Middleware *> preMiddlewares = URLService::searchRegexMapWithKey(endpointName, &endpointsPreMiddlewares);

    return runMiddlewares(preMiddlewares, req, res);
}

/**
 * @brief Runs the post-middlewares for a specific endpoint.
 *
 * This function retrieves the post-middlewares associated with the given endpoint name from the
 * `endpointsPostMiddlewares` map and calls the `runMiddlewares` function to execute them.
 *
 * @param endpointName The name of the endpoint for which to run the post-middlewares.
 * @return True if all post-middlewares were successfully executed, false otherwise.
 *
 * @note The post-middlewares are stored in the `endpointsPostMiddlewares` map, which is a map of endpoint names
 * to vectors of Middleware pointers.
 *
 * @see runMiddlewares
 * @author Ramy
 * @date 14-Aug-2024
 */
bool MiddlewareManager::runEndpointPostMiddleware(string endpointName, HTTPRequest *req, HTTPResponse *res)
{
    vector<Middleware *> postMiddlewares = URLService::searchRegexMapWithKey(endpointName, &endpointsPostMiddlewares);;
    return runMiddlewares(postMiddlewares, req, res);
}

MiddlewareManager::~MiddlewareManager()
{
    delete (sharedObjectPtr);
    for (auto m : this->middlewares)
        delete (m.second);
}