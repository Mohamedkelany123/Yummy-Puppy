#include "HTTPRequestManager.h"
#include "HTTPMethodNotAllowedExceptionHandler.h"
//Constructor: A Factory Manager class to avail different request method objects based on the request method type
HTTPRequestManager::HTTPRequestManager()
{
    methods ["GET"] = new HTTPGETRequest(NULL); // Instantiating HTTPGETRequest cloner
    methods ["POST"] = new HTTPPOSTRequest(NULL); // Instantiating HTTPPOSTRequest cloner
    methods ["OPTIONS"] = new HTTPOPTIONSRequest(NULL); // Instantiating HTTPOPTIONSRequest cloner

}
// Return a service whose type is based on the method. If the methodis not supported an exception is thrown
HTTPRequest * HTTPRequestManager::getService (TCPSocket * p_tcpSocket,string p_method)
{
    // Throw an exception if the method corresponding entry in the map cannot be found
    if ( methods[p_method]==NULL) throw (HTTPMethodNotAllowedExceptionHandler());
    return methods[p_method]->clone(p_tcpSocket); // else clone and return the object
}
// Destructor
HTTPRequestManager::~HTTPRequestManager()
{
    // A for_each iterator based loop with lambda function to deallocat all the cloner objects
    for_each (methods.begin(),methods.end(),[](const std::pair<string,HTTPRequest *>& it) -> bool {
        HTTPRequest * httpRequest = std::get<1>(it);
        delete(httpRequest);
        return true;
   });

}
