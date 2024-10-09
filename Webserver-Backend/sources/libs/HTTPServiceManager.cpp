#include "HTTPServiceManager.h"
#include "HTTPNotAcceptableExceptionHandler.h"
#include <regex>

#define WEB_CACHE_ROOT "./www"
// Constructor:  building up the factory map
/**
 * @brief Constructor for HTTPServiceManager class.
 *
 * This constructor initializes the HTTPServiceManager object by building up the factory map
 * using the provided configuration, logger, and middleware manager. It iterates through
 * the configuration items, loads the corresponding HTTPService from shared objects, and
 * assigns pre- and post-middlewares to each endpoint.
 *
 * @param conf Pointer to the ConfigFile object containing the server configuration.
 * @param logger Pointer to the Logger object for logging errors.
 * @param _middlewareManager Pointer to the MiddlewareManager object for managing middlewares.
 * @author Ramy
 * @date 14-Aug-2024
 */
HTTPServiceManager::HTTPServiceManager(ConfigFile *conf, Logger *logger, MiddlewareManager *_middlewareManager)
{
    sharedObjectPtr = new SharedObjectsManager<HTTPService>();
    for (auto &el : conf->data.items())
    {
        if (el.key() == "server_config")
            continue;
        std::cout << el.key() << " : ";
        try
        {
            string so_path = el.value()["so_path"];
            string http_path = el.value()["http_path"];
            vector<string> *parameters = URLService::getURLParams(http_path);
            http_path = URLService::getRegexURL(http_path);
            servicesParameters[http_path] = parameters;

            auto middlewares = el.value()["middlewares"];
            auto preMiddlewares = el.value()["middlewares"]["preMiddlewares"];
            auto postMiddlewares = el.value()["middlewares"]["postMiddlewares"];

            vector<string> endpointPreMiddlewares;
            for (auto &el : preMiddlewares.items())
            {
                cout << el.key() << ": " << el.value() << endl;
                endpointPreMiddlewares.push_back(el.value());
            }

            vector<string> endpointPostMiddlewares;
            for (auto &el : postMiddlewares.items())
            {
                cout << el.key() << ": " << el.value() << endl;
                endpointPostMiddlewares.push_back(el.value());
            }
            services[http_path] = sharedObjectPtr->load(so_path);
            _middlewareManager->assignEndpointPreMiddlewares(http_path, endpointPreMiddlewares);
            _middlewareManager->assignEndpointPostMiddlewares(http_path, endpointPostMiddlewares);
            regexURLs.push_back(http_path);
        }
        catch (exception e)
        {
            LOG_ERRORS(e.what());
        }
    }
}
// Compare the file extention to the map key first and if not found compare the whole file name
/**
 * @brief Retrieves the appropriate HTTPService based on the given resource.
 *
 * This function searches for the appropriate HTTPService based on the given resource
 * by comparing the resource with the regular expressions stored in the services map.
 * If a match is found, the corresponding HTTPService is cloned and returned. If no
 * match is found, an HTTPNotAcceptableExceptionHandler is thrown.
 *
 * @param p_resource The resource for which the HTTPService needs to be retrieved.
 * @return A pointer to the cloned HTTPService if a match is found, otherwise nullptr.
 * @throws HTTPNotAcceptableExceptionHandler If no match is found for the given resource.
 * @author Ramy
 * @date 14-Aug-2024
 */
HTTPService *HTTPServiceManager::getService(string p_resource)
{
    string ext = p_resource; //.substr(p_resource.find_last_of(".") + 1);
    pair<string, HTTPService *> service = URLService::searchRegexMapWithKey(p_resource, &services);
    if (service.second == nullptr)
    {
        throw(HTTPNotAcceptableExceptionHandler());
    }
    else
        return service.second->clone(); // clone service based on extension
}

/**
 * @brief Extracts URL parameters from the given URL based on the stored regular expressions.
 *
 * This function searches for the appropriate regular expression in the servicesParameters map
 * that matches the given URL. It then extracts the parameters from the URL and returns them
 * as a map of parameter names to their corresponding values.
 *
 * @param _url The URL from which to extract the parameters.
 * @return A pointer to a map containing the extracted parameter names and their values.
 *         If no parameters are found, an empty map is returned.
 *
 * @author Ramy
 * @date 14-Aug-2024
 */
map<string, string> *HTTPServiceManager::extractURLParams(string _url)
{
    pair<string, vector<string> *> parameters = URLService::searchRegexMapWithKey(_url, &servicesParameters);
    map<string, string> *parametersValues = new map<string, string>();
    if (parameters.second->size() == 0)
        return parametersValues;
    string regexURL = parameters.first;

    vector<string> * _urlSplit = URLService::splitURL(_url);
    vector<string> * regexURLSplit = URLService::splitURL(regexURL);

    int asteriskCount = 0;

    for (int i = 0; i < regexURLSplit->size(); i++)
    {
        if ((*regexURLSplit)[i] == ".*")
        {
            string parameterKey = (*URLService::splitURL(parameters.second->operator[](asteriskCount), ':'))[1];
            ;
            (*parametersValues)[parameterKey] = (*_urlSplit)[i];
            asteriskCount++;
        }
    }
    return parametersValues;
}


/**
 * @brief Extracts query parameters from the given URL.
 *
 * This function takes a URL as input, splits it at the '?' character to separate the
 * query parameters from the rest of the URL, and then further splits the query parameters
 * at the '&' character to separate individual key-value pairs. The function then stores
 * these key-value pairs in a map and returns a pointer to the map.
 *
 * @param _url The URL from which to extract the query parameters.
 * @return A pointer to a map containing the extracted query parameter names and their values.
 *         If no query parameters are found, an empty map is returned.
 *
 * @author Ramy
 * @date 9-Oct-2024
 */
map<string, string> *HTTPServiceManager::extractURLQueryParams(string _url)
{
    vector<string> * _urlSplit = URLService::splitURL(_url, '?');
    map<string, string> *parametersValues = new map<string, string>();

    // If the URL does not contain a '?', return an empty map
    if (_urlSplit->size() != 2)
        return parametersValues;

    // If the URL does not contain any query parameters, return an empty map
    if ((*_urlSplit)[1].empty())
        return parametersValues;

    vector<string> * _queryParamsSplit = URLService::splitURL((*_urlSplit)[1], '&');
    int asteriskCount = 0;

    // Iterate through the query parameters and store them in the map
    for (int i = 0; i < (*_queryParamsSplit).size(); i++)
    {
        vector<string> * _keyValuePair = URLService::splitURL((*_queryParamsSplit)[i], '=');

        // If a key-value pair does not contain an '=', return an empty map
        if (_keyValuePair->size()!= 2)
            return parametersValues;

        (*parametersValues)[(*_keyValuePair)[0]] = (*_keyValuePair)[1];
    }

    return parametersValues;
}
// Destructor
HTTPServiceManager::~HTTPServiceManager()
{
    // A for_each iterator based loop with lambda function to deallocate all the cloner objects
    for_each(services.begin(), services.end(), [](const std::pair<string, HTTPService *> &it) -> bool
             {
        HTTPService * httpService = std::get<1>(it);
        delete(httpService);
        return true; });
    for (auto m : this->servicesParameters)
        delete (m.second);
}