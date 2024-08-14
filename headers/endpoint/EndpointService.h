#include <PSQLController.h>
#include <common.h>
#include <PSQLAbstractQueryIterator.h>
#include <HTTPService.h>
#include <HTTPResponseHeader.h>
#include <Poco/JWT/JWT.h>
#include <Poco/JWT/Token.h>
#include <Poco/JWT/Signer.h>

using namespace Poco::JWT;

template <class I, class O>
class EndpointService : public HTTPService
{

    private:
        std::function<void(string http_body, I *inputSerializer, O *outputSerializer)> lambda;
        string endpoint_entry(string http_body, std::function<void(string http_body, I *inputSerializer, O *outputSerializer)> f)
        {

            I *inputSerializer = new I();
            O *outputSerializer = new O();

            inputSerializer->serialize(http_body);
            f(http_body, inputSerializer, outputSerializer);
            string str_return = outputSerializer->deserialize();
            delete (inputSerializer);
            delete (outputSerializer);
            return str_return;
        }

    public:
        EndpointService(std::function<void(string http_body, I *inputSerializer, O *outputSerializer)> _lambda) : HTTPService()
        {
            lambda = _lambda;
        }
        /**
         * Executes the HTTP request and response handling for the endpoint service.
         *
         * @param p_httpRequest Pointer to the HTTP request object.
         * @param p_httpResponse Pointer to the HTTP response object.
         * @param middlewareManager Pointer to the middleware manager object. Optional, defaults to NULL.
         *
         * @return True if the request is successfully processed and the response is written, false otherwise.
         */ 
        bool execute(HTTPRequest *p_httpRequest, HTTPResponse *p_httpResponse, MiddlewareManager *middlewareManager = NULL)
        {
            string data = p_httpRequest->getBody();
            middlewareManager->runEndpointPreMiddleware(p_httpRequest->getResource(), p_httpRequest, p_httpResponse);
            string reply = endpoint_entry(data, lambda);
            p_httpResponse->setBody(json::parse(reply));
            middlewareManager->runEndpointPostMiddleware(p_httpRequest->getResource(), p_httpRequest, p_httpResponse);
            p_httpResponse->write();
            return true;
        }
        // A pure virtual method that should be implemented by all descendants to clone and create new object
        HTTPService *clone()
        {
            return new EndpointService<I, O>(lambda);
        }
        ~EndpointService()
        {
        }
};
